/** \file
 * \brief Example code for Simple Open EtherCAT master
 *
 * Usage : simple_test [ifname1]
 * ifname is NIC interface, f.e. eth0
 *
 * This is a minimal test.
 *
 * (c)Arthur Ketels 2010 - 2011
 */

#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "ethercat.h"

#define EC_TIMEOUTMON 500

char IOmap[128];
int usedmen;
OSAL_THREAD_HANDLE thread1;
int expectedWKC;
boolean needlf;
volatile int wkc;
boolean inOP;
uint8 currentgroup = 0;
boolean forceByteAlignment = FALSE;

void set_output_int16 (uint16_t slave_nb, uint8_t module_index, int16_t value)
 {
  	uint8_t *data_ptr;
 
  	data_ptr = ec_slave[slave_nb].outputs;
  	/* Move pointer to correct module index*/
  	data_ptr += module_index * 2;
  	/* Read value byte by byte since all targets can't handle misaligned
     	   addresses */
  	*data_ptr++ = (value >> 0) & 0xFF;
  	*data_ptr++ = (value >> 8) & 0xFF;
 }

int16 get_input_int16(uint16_t slave_nb, uint8_t module_index, int16 *value)
 {
       uint8_t *data_ptr;
        
       data_ptr = ec_slave[slave_nb].inputs;
  	/* Move pointer to correct module index*/
  	data_ptr += module_index * 2;
    /* Read value byte by byte since all targets can't handle misaligned addresses */
    *value |= ((*data_ptr++) & 0xFF);
    *value |= ((*data_ptr) << 8) & 0xff00;

    return *value;
 }

void simpletest(char *ifname)
{
    //int i, j, oloop, iloop, chk;
    //needlf = FALSE;
    //inOP = FALSE;

   printf("Starting simple test\n");
   printf("Using adapter %s\n",ifname);

   /* initialise SOEM, bind socket to ifname */
    if (ec_init(ifname) > 0){}

    if ( ec_config_init(FALSE) > 0 )
    {
        printf("%d slaves found and configured.\n",ec_slavecount);
    }

    printf("Slave Name = %s\n",ec_slave[1].name);
    printf("Configured address: %x\n", ec_slave[1].SIIindex);
    //printf(" Inputs address: %x\n", (uint16_t*) ec_slave[1].inputs);

    //printf("Output bit: %x\n", get_input_int16(1,0,(int16_t*) 0x7000));
    printf("Output bit: %x\n", get_input_int16(1,0,0));
    //ecx_SDOread(1,0)

    usedmen = ec_config_map(&IOmap);

    printf("memory = %d\n", usedmen);
    
    
    /* send one valid process data to make outputs in slaves happy*/
    ec_send_processdata();
    wkc = ec_receive_processdata(EC_TIMEOUTRET);

    ec_writestate(0);
    /* wait for all slaves to reach OP state */
    ec_statecheck(0, EC_STATE_OPERATIONAL,  EC_TIMEOUTSTATE);

}
int main(int argc, char *argv[])
{
   printf("SOEM (Simple Open EtherCAT Master)\nSimple test\n");

   if (argc > 1)
   {
      /* start cyclic part */
      simpletest(argv[1]);
   }
   else
   {
      ec_adaptert * adapter = NULL;
      printf("Usage: simple_test ifname1\nifname = eth0 for example\n");

      printf ("\nAvailable adapters:\n");
      adapter = ec_find_adapters ();
      while (adapter != NULL)
      {
         printf ("    - %s  (%s)\n", adapter->name, adapter->desc);
         adapter = adapter->next;
      }
      ec_free_adapters(adapter);
   }

    printf("End simple test, close socket\n");
    /* stop SOEM, close socket */
    ec_close();
    printf("End program\n");
    return (0);
}