#pragma once
#include <Arduino.h>
#include <IPAddress.h>

extern "C" {
  #include "lwip/tcp.h"
  #include "lwip/ip_addr.h"
  #include "lwip/err.h"
}

class Arduino_10BASE_T1S_TCP {
public:
  Arduino_10BASE_T1S_TCP();

  // Server
  bool begin(uint16_t port);
  void stop();

  // Client
  bool connect(IPAddress ip, uint16_t port);

  // Stream-like API
  int     available() const;
  int     peek();
  int     read();
  int     read(uint8_t* buf, size_t len);
  size_t  write(const uint8_t* buf, size_t len);
  void    flush();

  bool     connected() const;
  IPAddress remoteIP() const;
  uint16_t  remotePort() const;

  // Buffers
  void setBufferSizes(size_t rxBytes, size_t txBytes);

private:
  // -------- ring buffer --------
  struct Ring {
    uint8_t* data   = nullptr;
    size_t   size   = 0;     // capacity
    size_t   rd     = 0;     // read index
    size_t   wr     = 0;     // write index
    size_t   count  = 0;     // used bytes
  };

  void   ringAlloc(Ring& r, size_t n);
  void   ringFree(Ring& r);
  size_t ringWrite(Ring& r, const uint8_t* buf, size_t len);
  size_t ringRead (Ring& r, uint8_t* buf, size_t len);
  int    ringPeek (const Ring& r) const;
  size_t ringPeekCopy(const Ring& r, uint8_t* dst, size_t len) const; // peek without consuming
  void   ringConsume(Ring& r, size_t len);                             // advance rd by len

  // -------- lwIP glue --------
  void   attach(struct tcp_pcb* pcb);
  void   detach();

  // Static callbacks (lwIP C API → C++)
  static err_t _onAccept   (void* arg, struct tcp_pcb* newpcb, err_t err);
  static err_t _onRecv     (void* arg, struct tcp_pcb* tpcb, struct pbuf* p, err_t err);
  static err_t _onSent     (void* arg, struct tcp_pcb* tpcb, u16_t len);
  static err_t _onPoll     (void* arg, struct tcp_pcb* tpcb);
  static void  _onErr      (void* arg, err_t err);
  static err_t _onConnected(void* arg, struct tcp_pcb* tpcb, err_t err);

  // Instance handlers
  err_t onAccept   (struct tcp_pcb* newpcb, err_t err);
  err_t onRecv     (struct tcp_pcb* tpcb, struct pbuf* p, err_t err);
  err_t onSent     (struct tcp_pcb* tpcb, u16_t len);
  err_t onPoll     (struct tcp_pcb* tpcb);
  void  onErr      (err_t err);
  err_t onConnected(struct tcp_pcb* tpcb, err_t err);

  // TX driver
  err_t tryPushTx();

  // -------- state --------
  bool            _isServer    = false;
  uint16_t        _listenPort  = 0;

  struct tcp_pcb* _listen      = nullptr; // server listen pcb
  struct tcp_pcb* _tpcb        = nullptr; // active connection pcb

  ip_addr_t       _remoteIp    {};        // peer IP
  uint16_t        _remotePort  = 0;       // peer port

  Ring            _rx;                    // RX ring buffer
  Ring            _tx;                    // TX ring buffer
};
