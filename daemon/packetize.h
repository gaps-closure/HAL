/* Include Packet or shared memory formats */
#include "packetize_sdh_ha_v1.h"
#include "packetize_sdh_be_v1.h"
#include "packetize_sdh_be_v2.h"
#include "packetize_sdh_be_v3.h"
#include "packetize_sdh_bw_v1.h"
#include "packetize_sdh_sm_v1.h"
/* Export conversion functions to device_read_write.c */
extern int  pdu_from_packet(pdu *, uint8_t *, int, device *);
extern void pdu_into_packet(uint8_t *out, pdu *in, int *pkt_len, selector *osel, device *odev);
