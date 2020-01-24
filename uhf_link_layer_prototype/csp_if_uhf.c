#include <stdint.h>
#include <csp/csp.h>
#include <csp/csp_interface.h>
#include <csp/drivers/uhf.h>
#include <csp/interfaces/csp_if_uhf.h>



static int csp_uhf_tx(csp_iface_t *interface, csp_packet_t *packet, uint32_t timeout){
    //convert csp_packet_t into data array, call uhf send
}

static int csp_uhf_init(){
  
}

/** Interface definition */
csp_iface_t csp_if_uhf = {
	.name = "UHF",
	.nexthop = csp_uhf_tx,
	.mtu = CSP_UHF_MTU,
};
