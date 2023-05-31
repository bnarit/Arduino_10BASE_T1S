// SPDX-License-Identifier: GPL-2.0+
/*
 * Driver for Microchip 10BASE-T1S PHYs
 *
 * Support: Microchip Phys:
 *  lan8670/1/2 Rev.B1
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/phy.h>

#define PHY_ID_LAN867X_REVB1 0x0007C162

#define LAN867X_REG_STS2 0x0019
#define LAN867x_RESET_COMPLETE_STS BIT(11)

#define LAN867X_DISABLE_COL_DET 0x0000
#define LAN867X_ENABLE_COL_DET 0x0083
#define LAN867X_REG_COL_DET_CTRL0 0x0087

/* PLCA enable - Could be configured by parameter */
static bool plca_enable = true;
/* PLCA node-id - Could configured by parameter */
static u8 plca_node_id = 0;
/* PLCA count - Could configured by parameter */
static u8 plca_node_count = 8;
/* Max burst count */
static u8 max_bc = 0x00;
/* Burst timer */
static u8 burst_timer = 0x80;
/* TO timer */
static u8 to_timer = 0x20;

module_param_named(enable, plca_enable, bool, 0660);
MODULE_PARM_DESC(enable, " PLCA Enable");
module_param_named(node_id, plca_node_id, byte, 0660);
MODULE_PARM_DESC(node_id, " PLCA Node-ID");
module_param_named(node_count, plca_node_count, byte, 0660);
MODULE_PARM_DESC(node_count, " PLCA Node-Count");
module_param_named(max_bc, max_bc, byte, 0660);
MODULE_PARM_DESC(max_bc, " Max Burst Count");
module_param_named(burst_timer, burst_timer, byte, 0660);
MODULE_PARM_DESC(burst_timer, " Burst Timer");
module_param_named(to_timer, to_timer, byte, 0660);
MODULE_PARM_DESC(to_timer, " TO Timer");

/* The arrays below are pulled from the following table from AN1699
 * Access MMD Address Value Mask
 * RMW 0x1F 0x00D0 0x0002 0x0E03
 * RMW 0x1F 0x00D1 0x0000 0x0300
 * RMW 0x1F 0x0084 0x3380 0xFFC0
 * RMW 0x1F 0x0085 0x0006 0x000F
 * RMW 0x1F 0x008A 0xC000 0xF800
 * RMW 0x1F 0x0087 0x801C 0x801C
 * RMW 0x1F 0x0088 0x033F 0x1FFF
 * W   0x1F 0x008B 0x0404 ------
 * RMW 0x1F 0x0080 0x0600 0x0600
 * RMW 0x1F 0x00F1 0x2400 0x7F00
 * RMW 0x1F 0x0096 0x2000 0x2000
 * W   0x1F 0x0099 0x7F80 ------
 */

static const u32 lan867x_revb1_fixup_registers[12] = {
	0x00D0, 0x00D1, 0x0084, 0x0085,
	0x008A, 0x0087, 0x0088, 0x008B,
	0x0080, 0x00F1, 0x0096, 0x0099,
};

static const u16 lan867x_revb1_fixup_values[12] = {
	0x0002, 0x0000, 0x3380, 0x0006,
	0xC000, 0x801C, 0x033F, 0x0404,
	0x0600, 0x2400, 0x2000, 0x7F80,
};

static const u16 lan867x_revb1_fixup_masks[12] = {
	0x0E03, 0x0300, 0xFFC0, 0x000F,
	0xF800, 0x801C, 0x1FFF, 0xFFFF,
	0x0600, 0x7F00, 0x2000, 0xFFFF,
};

static int lan867x_configure_plca(struct phy_device *phydev)
{
	int ret;

	ret = phy_write_mmd(phydev, MDIO_MMD_VEND2, 0xCA02, plca_node_count << 8 | plca_node_id);
	if (ret < 0)
		return ret;
	ret = phy_write_mmd(phydev, MDIO_MMD_VEND2, 0xCA05, max_bc << 8 | burst_timer);
	if (ret < 0)
		return ret;
	ret = phy_write_mmd(phydev, MDIO_MMD_VEND2, 0xCA04, to_timer);
	if (ret < 0)
		return ret;
	if (plca_enable) {
		ret = phy_write_mmd(phydev, MDIO_MMD_VEND2, 0xCA01, 0x8000);
		if (ret < 0)
			return ret;
	} else {
		ret = phy_write_mmd(phydev, MDIO_MMD_VEND2, 0xCA01, 0x0000);
		if (ret < 0)
			return ret;
	}
	if (plca_enable) {
		ret = phy_write_mmd(phydev, MDIO_MMD_VEND2, LAN867X_REG_COL_DET_CTRL0, LAN867X_DISABLE_COL_DET);
		if (ret < 0)
			return ret;
		phydev_info(phydev, "PLCA mode enabled. Node Id: %d, Node Count: %d, Max BC: %d, Burst Timer: %d, TO Timer: %d\n",
			    plca_node_id, plca_node_count, max_bc, burst_timer, to_timer);
	} else {
		ret = phy_write_mmd(phydev, MDIO_MMD_VEND2, LAN867X_REG_COL_DET_CTRL0, LAN867X_ENABLE_COL_DET);
		if (ret < 0)
			return ret;
		phydev_info(phydev, "CSMA/CD mode enabled\n");
	}

	return 0;
}

static int lan867x_revb1_config_init(struct phy_device *phydev)
{
	int ret;

	/* Read STS2 register and check for the Reset Complete status to do the
	 * init configuration. If the Reset Complete is not set, wait for 5us
	 * and then read STS2 register again and check for Reset Complete status.
	 * Still if it is failed then declare PHY reset error or else proceed
	 * for the PHY initial register configuration.
	 */
	ret = phy_read_mmd(phydev, MDIO_MMD_VEND2, LAN867X_REG_STS2);
	if (ret < 0)
		return ret;

	if (!(ret & LAN867x_RESET_COMPLETE_STS)) {
		udelay(5);
		ret = phy_read_mmd(phydev, MDIO_MMD_VEND2, LAN867X_REG_STS2);
		if (ret < 0)
			return ret;
		if (!(ret & LAN867x_RESET_COMPLETE_STS)) {
			phydev_err(phydev, "PHY reset failed\n");
			return -ENODEV;
		}
	}

	/* Read-Modified Write Pseudocode (from AN1699)
	 * current_val = read_register(mmd, addr) // Read current register value
	 * new_val = current_val AND (NOT mask) // Clear bit fields to be written
	 * new_val = new_val OR value // Set bits
	 * write_register(mmd, addr, new_val) // Write back updated register value
	 */
	for (int i = 0; i < ARRAY_SIZE(lan867x_revb1_fixup_registers); i++) {
		ret = phy_modify_mmd(phydev, MDIO_MMD_VEND2,
				     lan867x_revb1_fixup_registers[i],
				     lan867x_revb1_fixup_masks[i],
				     lan867x_revb1_fixup_values[i]);
		if (ret)
			return ret;
	}

	return lan867x_configure_plca(phydev);
}

static int lan867x_read_status(struct phy_device *phydev)
{
	/* The phy has some limitations, namely:
	 *  - always reports link up
	 *  - only supports 10MBit half duplex
	 *  - does not support auto negotiate
	 */
	phydev->link = 1;
	phydev->duplex = DUPLEX_HALF;
	phydev->speed = SPEED_10;
	phydev->autoneg = AUTONEG_DISABLE;

	return 0;
}

static struct phy_driver microchip_t1s_driver[] = {
	{
		PHY_ID_MATCH_EXACT(PHY_ID_LAN867X_REVB1),
		.name               = "LAN867X Rev.B1",
		.config_init        = lan867x_revb1_config_init,
		.read_status        = lan867x_read_status,
	},
};

module_phy_driver(microchip_t1s_driver);

static struct mdio_device_id __maybe_unused tbl[] = {
	{ PHY_ID_MATCH_EXACT(PHY_ID_LAN867X_REVB1) },
	{ }
};

MODULE_DEVICE_TABLE(mdio, tbl);

MODULE_DESCRIPTION("Microchip 10BASE-T1S PHYs driver");
MODULE_AUTHOR("Parthiban Veerasooran <Parthiban.Veerasooran@microchip.com>");
MODULE_LICENSE("GPL");
