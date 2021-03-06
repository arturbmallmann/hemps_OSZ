/*!\file osz.h
 * HEMPS VERSION - 8.5 - support for OSZ
 *
 * Distribution:  August 2020
 *
 * Created by: Luciano Caimi - contact: lcaimi@gmail.com
 *             Rafael Faccenda - contact: faccendarafael@gmail.com
 *
 * Research group: GAPH-PUCRS   -  contact:  fernando.moraes@pucrs.br
 *
 * \brief This module implements functions to open and close Opaque Secure Zones
 *
 */

#include "osz_slave.h"
#include "utils.h"
#include "define_pairs.h"
#include "seek.h" 

int wrapper_value = 0;
int LOCAL_left_low_corner = -1;
int LOCAL_right_high_corner = -1;

//////////////////////////////////////////////////////////////////////////////////////
void Set_Secure_Zone(unsigned int left_low_corner, unsigned int right_high_corner, unsigned int master_PE){
	unsigned int my_X_addr, my_Y_addr;

	unsigned int RH_X_addr, RH_Y_addr;
	unsigned int LL_X_addr, LL_Y_addr;
	int isolated_ports = 0;

	my_X_addr = (get_net_address() & 0xF00) >> 8;
	my_Y_addr = get_net_address() & 0x00F;

	RH_X_addr = (right_high_corner & 0xF0) >> 4;
	RH_Y_addr = right_high_corner & 0x0F;

	LL_X_addr = (left_low_corner & 0xF0) >> 4;
	LL_Y_addr = left_low_corner & 0x0F;

	if((my_X_addr == RH_X_addr) && (my_Y_addr == RH_Y_addr)){
		Seek(CLEAR_SERVICE, master_PE, master_PE, 0);
	}

	// puts("X");puts(itoh(my_X_addr));puts(" ");
	// puts("Y");puts(itoh(my_Y_addr));puts("\n");

	// puts("LL_X");puts(itoh(LL_X_addr));puts(" ");
	// puts("LL_Y");puts(itoh(LL_Y_addr));puts("\n");

	// puts("RH_X");puts(itoh(RH_X_addr));puts(" ");
	// puts("RH_Y");puts(itoh(RH_Y_addr));puts("\n");

	//WEST or EAST test
	if(my_Y_addr <= RH_Y_addr && my_Y_addr >= LL_Y_addr ){
		if(my_X_addr == RH_X_addr){//EAST
			// puts("E border\n");
			isolated_ports = 0x3;
		}
		if(my_X_addr == LL_X_addr){//WEST
			// puts("W border\n");
			isolated_ports = 0xC;
		}
	}

	//NORTH or SOUTH test
	if(my_X_addr >= LL_X_addr && my_X_addr <= RH_X_addr){
		if(my_Y_addr == RH_Y_addr){//NORTH
			// puts("N border\n");
			isolated_ports = 0x30 + isolated_ports;
		}
		if(my_Y_addr == LL_Y_addr){//SOUTH
			// puts("S border\n");
			isolated_ports = 0xC0 + isolated_ports;
		}
	}
	if(isolated_ports != 0){
		wrapper_value = isolated_ports;
		//MemoryWrite(WRAPPER_REGISTER,isolated_ports);
		LOCAL_left_low_corner = left_low_corner;
		LOCAL_right_high_corner = right_high_corner;
		if((my_X_addr == RH_X_addr) && (my_Y_addr == RH_Y_addr)){
			Seek(SET_SZ_RECEIVED_SERVICE, get_net_address(), master_PE, right_high_corner);
			puts("SET SZ RH: ");puts(itoh(LOCAL_right_high_corner));puts("\n");	
		}
		seek_puts("wrapper: ");seek_puts(itoh(isolated_ports));seek_puts("\n");
	}
}


//////////////////////////////////////////////////////////////////////////////////////
void Unset_Secure_Zone(unsigned int left_low_corner, unsigned int right_high_corner, unsigned int master_PE){
  unsigned int my_X_addr, my_Y_addr, master_X_addr, master_Y_addr;

  unsigned int RH_X_addr, RH_Y_addr, LOCAL_RH_X_addr, LOCAL_RH_Y_addr;
  unsigned int LL_X_addr, LL_Y_addr, LOCAL_LL_X_addr, LOCAL_LL_Y_addr;
  int isolated_ports, previous_isolated;

  my_X_addr = (get_net_address() & 0xF00) >> 8;
  my_Y_addr = get_net_address() & 0x00F;

  master_X_addr = (master_PE & 0xF0) >> 4;
  master_Y_addr = master_PE & 0x00F;

  LOCAL_RH_X_addr = (LOCAL_right_high_corner & 0xF0) >> 4;
  LOCAL_RH_Y_addr = LOCAL_right_high_corner & 0x0F;

  LOCAL_LL_X_addr = (LOCAL_left_low_corner & 0xF0) >> 4;
  LOCAL_LL_Y_addr = LOCAL_left_low_corner & 0x0F;

  RH_X_addr = (right_high_corner & 0xF0) >> 4;
  RH_Y_addr = right_high_corner & 0x0F;

  LL_X_addr = (left_low_corner & 0xF0) >> 4;
  LL_Y_addr = left_low_corner & 0x0F;

  int myOSZ = 0;
  int noCut = 0;

  if ((right_high_corner == left_low_corner) && (LOCAL_right_high_corner == right_high_corner)){
    noCut = 1;
    puts("Não tem corte\n");
  }

  if (noCut){ //Caso não precise de CUT
    if((my_X_addr == LOCAL_RH_X_addr) && (my_Y_addr == LOCAL_RH_Y_addr)){ //Se eu for RH local
      if((my_X_addr == RH_X_addr) && (my_Y_addr == RH_Y_addr)){     //Se veio o indice do meu RH local na mensagem de CUT
        myOSZ = 1;
        Seek(CLEAR_SERVICE, master_PE, master_PE, 0);
        puts("Não tem corte e é minha zona segura\n");
      }
    }
  }else{    //Caso precise de CUT
    if((my_X_addr == LOCAL_RH_X_addr) && (my_Y_addr == LOCAL_RH_Y_addr)){ //Se eu for RH local
      if((LL_X_addr == LOCAL_LL_X_addr) && (LL_Y_addr == LOCAL_LL_Y_addr)){ //Se for no meu LL
        myOSZ = 1;
        Seek(CLEAR_SERVICE, master_PE, master_PE, 0);
        puts("Tem corte e é minha zona segura\n");
      }
    }
  }

  // send 
  // if((my_X_addr == LOCAL_RH_X_addr) && (my_Y_addr == LOCAL_RH_Y_addr)){
  //   Seek(CLEAR_SERVICE, master_PE, master_PE, 0);
  // }


  // puts("X");puts(itoh(my_X_addr));puts(" ");
  // puts("Y");puts(itoh(my_Y_addr));puts("\n");

  // puts("LL_X");puts(itoh(LL_X_addr));puts(" ");
  // puts("LL_Y");puts(itoh(LL_Y_addr));puts("\n");

  // puts("RH_X");puts(itoh(RH_X_addr));puts(" ");
  // puts("RH_Y");puts(itoh(RH_Y_addr));puts("\n");

  // read actual wrapper value
  isolated_ports = wrapper_value;
  previous_isolated = wrapper_value;


  //seek_puts("previous wrapper: "); seek_puts(itoh(wrapper_value)); seek_puts("\n");
  if(wrapper_value == 0)
  	return;

  if (noCut && myOSZ){
  //if((my_X_addr == LOCAL_RH_X_addr) && (my_Y_addr == LOCAL_RH_Y_addr)){   
      	  right_high_corner = ((get_net_address() >> 4)& 0XF0) | (get_net_address() &  0X0F);
          Seek(SECURE_ZONE_CLOSED_SERVICE, get_net_address(), master_PE, LOCAL_right_high_corner);
          //puts("ENDSZ RH:");puts(itoh(LOCAL_right_high_corner));puts("\n"); 
          seek_puts("Without CUT - wrapper: ");seek_puts(itoh(isolated_ports));seek_puts("\n");
          seek_puts("RH address: ");seek_puts(itoh(right_high_corner));seek_puts("\n");
          return;
  }

// This is to uncut at RH position
//  // set wrapper port EAST
//  if( (my_X_addr == LL_X_addr - 1) && (my_Y_addr >=  LL_Y_addr) &&  (my_Y_addr <=  RH_Y_addr) )
//      isolated_ports = isolated_ports + 0x3;
//
//  // set wrapper port NORTH
//  if( (my_Y_addr == LL_Y_addr - 1) && (my_X_addr == LL_X_addr ))
//      isolated_ports = isolated_ports + 0x30;
//
//// UNSET wrapper port EAST
//  if( (my_X_addr == LL_X_addr) && (my_Y_addr >=  LL_Y_addr) &&  (my_Y_addr <=  RH_Y_addr) )
//      isolated_ports = isolated_ports - 0x3;
//
//  // UNSET wrapper port NORTH
//  if( (my_Y_addr == RH_Y_addr) && (my_X_addr == RH_X_addr ))
//      isolated_ports = isolated_ports - 0x30;    


// This is to uncut at LL position
// set wrapper port WEST
  if( (my_X_addr == LL_X_addr + 1) && (my_Y_addr >=  LL_Y_addr) &&  (my_Y_addr <=  RH_Y_addr) )
      isolated_ports = isolated_ports + 0x0C;

  // set wrapper port SOUTH
  if( (my_Y_addr == RH_Y_addr + 1) && (my_X_addr == RH_X_addr))
      isolated_ports = isolated_ports + 0xC0;

// UNSET wrapper port WEST
  if( (my_X_addr == LL_X_addr) && (my_Y_addr >=  LL_Y_addr) &&  (my_Y_addr <=  RH_Y_addr) )
      isolated_ports = isolated_ports - 0x0C;

  // UNSET wrapper port SOUTH
  if( (my_Y_addr == LL_Y_addr) && (my_X_addr == LL_X_addr ))
      isolated_ports = isolated_ports - 0xC0;    


  //if(isolated_ports != previous_isolated){
  	seek_puts("write wrapper: ");seek_puts(itoh(isolated_ports));seek_puts("\n");
    MemoryWrite(WRAPPER_REGISTER,isolated_ports);
    if(myOSZ){
    //if((my_X_addr == LOCAL_RH_X_addr) && (my_Y_addr == LOCAL_RH_Y_addr)){
      Seek(SECURE_ZONE_CLOSED_SERVICE, get_net_address(), master_PE, LOCAL_right_high_corner);
      puts("ENDSZ RH:");puts(itoh(LOCAL_right_high_corner));puts("\n"); 
    }
    //seek_puts("wrapper:");seek_puts(itoh(isolated_ports));seek_puts("\n");
  //}
  wrapper_value = isolated_ports;
  seek_puts("RH address: ");seek_puts(itoh(right_high_corner));seek_puts("\n");
  seek_puts("LOCAL RH address: ");seek_puts(itoh(LOCAL_right_high_corner));seek_puts("\n");
}
