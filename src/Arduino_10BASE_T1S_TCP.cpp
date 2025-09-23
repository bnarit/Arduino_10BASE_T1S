#include "Arduino_10BASE_T1S_TCP.h"

#ifndef TCP_WRITE_CHUNK
#define TCP_WRITE_CHUNK (1460)  // conservative; <= sndbuf/MSS
#endif
//#define DEBUG_SHOW_TCP_STATE

Arduino_10BASE_T1S_TCP::Arduino_10BASE_T1S_TCP() {
  if (!ringAlloc(_rx, kDefaultRxBytes)) {
    //Serial.println("TCP: failed to allocate RX buffer");
    //Serial.flush();
  }
  if (!ringAlloc(_tx, kDefaultTxBytes)) {
    //Serial.println("TCP: failed to allocate TX buffer");
    //Serial.flush();
    ringFree(_rx);
  }
}

Arduino_10BASE_T1S_TCP::~Arduino_10BASE_T1S_TCP() {
  ringFree(_rx);
  ringFree(_tx);
}

void Arduino_10BASE_T1S_TCP::setBufferSizes(size_t rx, size_t tx) {
  if (!rx || !tx) {
    //Serial.println("TCP: buffer sizes must be non-zero");
    //Serial.flush();
    return;
  }
  if (_rx.size == rx && _tx.size == tx) return;

  Ring newRx{};
  Ring newTx{};

  bool rxOk = ringAlloc(newRx, rx);
  bool txOk = ringAlloc(newTx, tx);

  if (!rxOk || !txOk) {
    //Serial.println("TCP: buffer resize failed");
    //Serial.flush();
    ringFree(newRx);
    ringFree(newTx);
    return;
  }

  ringFree(_rx);
  ringFree(_tx);
  _rx = newRx;
  _tx = newTx;
  newRx = Ring{};
  newTx = Ring{};
}

// -------- ring buffer --------

bool Arduino_10BASE_T1S_TCP::ringAlloc(Ring& r, size_t n) {
  ringFree(r);
  if (!n) return false;

  r.data = static_cast<uint8_t*>(malloc(n));
  if (!r.data) {
    r.size = 0;
    return false;
  }
  r.size = n;
  r.rd = r.wr = r.count = 0;
  return true;
}
void Arduino_10BASE_T1S_TCP::ringFree(Ring& r) {
  if (r.data) free(r.data);
  r = Ring{};
}
size_t Arduino_10BASE_T1S_TCP::ringWrite(Ring& r, const uint8_t* buf, size_t len) {
  if (!r.data || !len) return 0;
  size_t copied = 0;
  while (copied < len && r.count < r.size) {
    r.data[r.wr] = buf[copied++];
    r.wr = (r.wr + 1) % r.size;
    r.count++;
  }
  return copied;
}
size_t Arduino_10BASE_T1S_TCP::ringRead(Ring& r, uint8_t* buf, size_t len) {
  if (!r.data || !len) return 0;
  size_t out = 0;
  while (out < len && r.count) {
    buf[out++] = r.data[r.rd];
    r.rd = (r.rd + 1) % r.size;
    r.count--;
  }
  return out;
}
int Arduino_10BASE_T1S_TCP::ringPeek(const Ring& r) const {
  if (!r.data || !r.count) return -1;
  return r.data[r.rd];
}

// Peek-copy up to `len` bytes from ring **without** advancing rd.
// Copies across wrap if needed.
size_t Arduino_10BASE_T1S_TCP::ringPeekCopy(const Ring& r, uint8_t* dst, size_t len) const {
  if (!r.data || !len) return 0;
  size_t toCopy = (r.count < len) ? r.count : len;
  if (!toCopy) return 0;
  size_t first = toCopy;
  size_t untilEnd = r.size - r.rd;
  if (first > untilEnd) first = untilEnd;
  memcpy(dst, r.data + r.rd, first);
  size_t remain = toCopy - first;
  if (remain) memcpy(dst + first, r.data, remain);
  return toCopy;
}

// Advance rd by len (assumes len <= r.count)
void Arduino_10BASE_T1S_TCP::ringConsume(Ring& r, size_t len) {
  if (!len || !r.count) return;
  if (len > r.count) len = r.count;
  r.rd = (r.rd + len) % r.size;
  r.count -= len;
}

// ---------- lwIP glue ----------

bool Arduino_10BASE_T1S_TCP::begin(uint16_t port) {
  stop();
  _isServer = true;
  _listenPort = port;

  struct tcp_pcb* pcb = tcp_new_ip_type(IPADDR_TYPE_V4);
  if (!pcb) return false;

  err_t e = tcp_bind(pcb, IP_ANY_TYPE, port);
  if (e != ERR_OK) {
    Serial.println("tcp_bind failed"); Serial.flush();
    tcp_close(pcb);
    return false;
  }
  _listen = tcp_listen_with_backlog(pcb, 4);
  if (!_listen) {
    Serial.println("tcp_listen_with_backlog failed"); Serial.flush();
    tcp_close(pcb);
    return false;
  }
  tcp_arg(_listen, this);
  tcp_accept(_listen, &_onAccept);
  Serial.println("TCP server listening on port " + String(port));
  Serial.flush();
  return true;
}

// Connected callback for client mode
err_t Arduino_10BASE_T1S_TCP::_onConnected(void* arg, struct tcp_pcb* tpcb, err_t err) {
  auto self = reinterpret_cast<Arduino_10BASE_T1S_TCP*>(arg);
  return self ? self->onConnected(tpcb, err) : ERR_OK;
}
err_t Arduino_10BASE_T1S_TCP::onConnected(struct tcp_pcb* tpcb, err_t err) {
  if (err != ERR_OK) {
    return err;
  }
  attach(tpcb);
  _remoteIp   = tpcb->remote_ip;
  _remotePort = tpcb->remote_port;
  return ERR_OK;
}

bool Arduino_10BASE_T1S_TCP::connect(IPAddress ip, uint16_t port) {
  stop();
  Serial.println("TCP connect to " + ip.toString() + ":" + String(port));
  Serial.flush();
  _isServer = false;

  struct tcp_pcb* pcb = tcp_new_ip_type(IPADDR_TYPE_V4);
  if (!pcb) return false;

  // Attach *before* connect so callbacks have context
  tcp_arg(pcb, this);
  tcp_recv(pcb, &_onRecv);
  tcp_err (pcb, &_onErr);
  tcp_poll(pcb, &_onPoll, 1);
  tcp_sent(pcb, &_onSent);
  tcp_nagle_disable(pcb);

  ip4_addr_t dip;
  ip4addr_aton(ip.toString().c_str(), &dip);

  err_t e = tcp_connect(pcb, ip_2_ip4(&dip), port, &_onConnected);
  if (e != ERR_OK && e != ERR_INPROGRESS) {
    tcp_abort(pcb);
    return false;
  }

  _tpcb = pcb;  // saved until onConnected finalizes
  return true;
}

void Arduino_10BASE_T1S_TCP::attach(struct tcp_pcb* pcb) {
  _tpcb = pcb;
  tcp_arg(_tpcb, this);
  tcp_recv(_tpcb, &_onRecv);
  tcp_err (_tpcb, &_onErr);
  tcp_poll(_tpcb, &_onPoll, 1);       // poll every tick (low latency)
  tcp_sent(_tpcb, &_onSent);          // get ACK notifications
  tcp_nagle_disable(_tpcb);           // low-latency small writes
}

void Arduino_10BASE_T1S_TCP::detach() {
  _tpcb = nullptr;
  _remotePort = 0;
  IP_SET_TYPE_VAL(_remoteIp, IPADDR_TYPE_V4);
  ip4_addr_set_u32(ip_2_ip4(&_remoteIp), 0);
}

void Arduino_10BASE_T1S_TCP::stop() {
  if (_tpcb) {
    // clear callbacks to avoid stray calls into this after close/abort
    tcp_arg(_tpcb, nullptr);
    tcp_recv(_tpcb, nullptr);
    tcp_err (_tpcb, nullptr);
    tcp_poll(_tpcb, nullptr, 0);
    tcp_sent(_tpcb, nullptr);
    if (tcp_close(_tpcb) != ERR_OK) {
      tcp_abort(_tpcb);
    }
    detach();
  }
  if (_listen) {
    tcp_arg(_listen, nullptr);
    tcp_accept(_listen, nullptr);
    tcp_close(_listen);
    _listen = nullptr;
  }
  _isServer = false;
  _listenPort = 0;
  _rx.rd = _rx.wr = _rx.count = 0;
  _tx.rd = _tx.wr = _tx.count = 0;
}

int Arduino_10BASE_T1S_TCP::available() const { return (int)_rx.count; }
int Arduino_10BASE_T1S_TCP::peek() { return ringPeek(_rx); }

int Arduino_10BASE_T1S_TCP::read() {
  uint8_t b;
  if (ringRead(_rx, &b, 1) == 1) return (int)b;
  return -1;
}

int Arduino_10BASE_T1S_TCP::read(uint8_t* buf, size_t len) {
  return (int)ringRead(_rx, buf, len);
}

size_t Arduino_10BASE_T1S_TCP::write(const uint8_t* buf, size_t len) {
  if (!_tpcb || !len) return 0;
  size_t copied = ringWrite(_tx, buf, len);
  if (!copied) {
    // Serial.println("TCP write: TX ring full");
    return 0;
  }

  //Serial.print("TCP queued bytes=");
  //Serial.println(copied);
  tryPushTx();
  return copied;
}

void Arduino_10BASE_T1S_TCP::flush() {
  if (_tpcb) tryPushTx();
}

bool Arduino_10BASE_T1S_TCP::connected() const {
  if (!_tpcb) return false;
  return (_tpcb->state == ESTABLISHED) || (_tpcb->state == CLOSE_WAIT);
}

IPAddress Arduino_10BASE_T1S_TCP::remoteIP() const {
  if (!_tpcb) return IPAddress(0,0,0,0);
  if (IP_IS_V4_VAL(_tpcb->remote_ip)) {
    auto u = ip4_addr_get_u32(ip_2_ip4(&_tpcb->remote_ip));
    return IPAddress(u);
  }
  return IPAddress(0,0,0,0);
}

uint16_t Arduino_10BASE_T1S_TCP::remotePort() const {
  if (!_tpcb) return 0;
  return _tpcb->remote_port;
}

// ---------- callbacks ----------

err_t Arduino_10BASE_T1S_TCP::_onAccept(void* arg, struct tcp_pcb* newpcb, err_t err) {
  auto self = reinterpret_cast<Arduino_10BASE_T1S_TCP*>(arg);
  return self ? self->onAccept(newpcb, err) : ERR_OK;
}
err_t Arduino_10BASE_T1S_TCP::_onRecv(void* arg, struct tcp_pcb* tpcb, struct pbuf* p, err_t err) {
  auto self = reinterpret_cast<Arduino_10BASE_T1S_TCP*>(arg);
  return self ? self->onRecv(tpcb, p, err) : ERR_OK;
}
err_t Arduino_10BASE_T1S_TCP::_onSent(void* arg, struct tcp_pcb* tpcb, u16_t len) {
  auto self = reinterpret_cast<Arduino_10BASE_T1S_TCP*>(arg);
  return self ? self->onSent(tpcb, len) : ERR_OK;
}
err_t Arduino_10BASE_T1S_TCP::_onPoll(void* arg, struct tcp_pcb* tpcb) {
  auto self = reinterpret_cast<Arduino_10BASE_T1S_TCP*>(arg);
  return self ? self->onPoll(tpcb) : ERR_OK;
}
void Arduino_10BASE_T1S_TCP::_onErr(void* arg, err_t err) {
  auto self = reinterpret_cast<Arduino_10BASE_T1S_TCP*>(arg);
  if (self) self->onErr(err);
}

err_t Arduino_10BASE_T1S_TCP::onAccept(struct tcp_pcb* newpcb, err_t err) {
  LWIP_UNUSED_ARG(err);
#ifdef DEBUG_SHOW_TCP_STATE
  Serial.printf("onAccept err=%d newpcb=%p\n", err, (void*)newpcb);
  Serial.flush();
#endif
  if (_tpcb) {   // single-connection server
    tcp_abort(newpcb);
    return ERR_ABRT;
  }
  attach(newpcb);
  _remoteIp   = newpcb->remote_ip;
  _remotePort = newpcb->remote_port;
  return ERR_OK;
}

err_t Arduino_10BASE_T1S_TCP::onRecv(struct tcp_pcb* tpcb, struct pbuf* p, err_t err) {
#ifdef DEBUG_SHOW_TCP_STATE
  Serial.printf("onRecv err=%d p=%p\n", err, (void*)p);
  Serial.flush();
#endif
  if (err != ERR_OK) {
    if (p) pbuf_free(p);
    return err;
  }
  if (!p) {
    // Peer sent FIN → acknowledge by closing our side, lwIP will sequence this correctly.
    tcp_close(tpcb); // if ERR_MEM, lwIP will retry close via poll
#ifdef DEBUG_SHOW_TCP_STATE
    Serial.printf("onClose tpcb=%p\n", (void*)tpcb);
#endif
    detach();
    return ERR_OK;
  }

  // Copy into RX ring with backpressure.
  // Only ack (tcp_recved) as many bytes as we successfully buffered.
  size_t total_wrote = 0;
  for (struct pbuf* q = p; q; q = q->next) {
    const uint8_t* d = (const uint8_t*)q->payload;
    size_t wrote = ringWrite(_rx, d, q->len);
    total_wrote += wrote;

    if (wrote < q->len) {
      // RX ring is full; stop here. We *do not* ack the remaining bytes,
      // so the sender's window will throttle/retry.
      break;
    }
  }

  if (total_wrote) {
    tcp_recved(tpcb, (u16_t)total_wrote);
  }
  // We must still free the pbuf chain we got from lwIP.
  pbuf_free(p);

  // Try to push any pending TX (e.g., echo/response)
  tryPushTx();
  return ERR_OK;
}

err_t Arduino_10BASE_T1S_TCP::onSent(struct tcp_pcb* tpcb, u16_t len) {
  LWIP_UNUSED_ARG(tpcb);
  //Serial.print("TCP ACK len=");
  //Serial.println(len);
  // ACK notification -> queue more if pending
  tryPushTx();
  return ERR_OK;
}

err_t Arduino_10BASE_T1S_TCP::onPoll(struct tcp_pcb* tpcb) {
  LWIP_UNUSED_ARG(tpcb);
  // Retry writes; also useful for keepalives/timeouts
  tryPushTx();
  return ERR_OK;
}

void Arduino_10BASE_T1S_TCP::onErr(err_t err) {
  LWIP_UNUSED_ARG(err);
  detach(); // connection gone
}

// Only consume TX ring after a successful tcp_write()
// Use TCP_WRITE_FLAG_MORE when more data remains.
err_t Arduino_10BASE_T1S_TCP::tryPushTx() {
  if (!_tpcb || !_tx.count) return ERR_OK;

  err_t res = ERR_OK;
  while (_tx.count) {
    u16_t can = tcp_sndbuf(_tpcb);
    if (!can) {
      // Serial.println("TCP tryPushTx: sndbuf=0");
      break;
    }

    // Peek-copy up to min(queue, can, TCP_WRITE_CHUNK)
    uint8_t buf[TCP_WRITE_CHUNK];
    size_t want = _tx.count;
    if (want > TCP_WRITE_CHUNK) want = TCP_WRITE_CHUNK;
    if (want > can)            want = can;

    size_t got = ringPeekCopy(_tx, buf, want);
    if (!got) break;

    // If more remains after this chunk, mark MORE
    size_t remain_after = _tx.count - got;
    u8_t flags = TCP_WRITE_FLAG_COPY | (remain_after ? TCP_WRITE_FLAG_MORE : 0);

    res = tcp_write(_tpcb, buf, (u16_t)got, flags);
    if (res == ERR_MEM) {
      // Serial.println("TCP tryPushTx: tcp_write ERR_MEM");
      break;
    }
    if (res != ERR_OK) {
      // Serial.print("TCP tryPushTx: tcp_write err=");
      // Serial.println(res);
      break;
    }

    // Success: now consume from ring
    ringConsume(_tx, got);
    //Serial.print("TCP TX bytes=");
    //Serial.print(got);
    //Serial.print(" at ms=");
    //Serial.println(millis());

    // If send buffer is now full, stop; we'll continue on next ACK/poll
    if (!tcp_sndbuf(_tpcb)) break;
  }

  if (res == ERR_OK) {
    tcp_output(_tpcb);  // flush queued segments now
  }
  return res;
}
