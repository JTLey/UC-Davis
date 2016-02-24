 module lab1(
 ////////////////
 ///FPGA Pins
 /////////////
 
 //Clock Pins
 CLOCK_50,
 
 //Seven Segment displays
 HEX0, HEX1, HEX2, HEX3, HEX4, HEX5,
 
 //Pushbuttons
 KEY,
 
 //LEDs
 LEDR,
 
 //Slider Switches
 SW,
 
 ///////////
 //MPS Pins
 //////////
 
 //DDR3 SDRAM
 HPS_DDR3_ADDR,
 HPS_DDR3_BA,
 HPS_DDR3_CAS_N,
 HPS_DDR3_CKE,
 HPS_DDR3_CK_N,
 HPS_DDR3_CK_P,
 HPS_DDR3_CS_N,
 HPS_DDR3_DM,
 HPS_DDR3_DQ,
 HPS_DDR3_DQS_N,
 HPS_DDR3_DQS_P,
 HPS_DDR3_ODT,
 HPS_DDR3_RAS_N,
 HPS_DDR3_RESET_N,
 HPS_DDR3_RZQ,
 HPS_DDR3_WE_N,
 );
 
 //********Port declartions
 
 // 1.FPGA Pins
 
 input CLOCK_50;
 output [6:0] HEX0, HEX1, HEX2, HEX3, HEX4, HEX5;
 input [3:0] KEY;
 output[9:0] LEDR;
 input [9:0] SW;

// 2.HPS Pins

//DDR3 SDRAM
output [14:0] HPS_DDR3_ADDR;
output [2:0] HPS_DDR3_BA;
output HPS_DDR3_CAS_N;
output HPS_DDR3_CKE;
output HPS_DDR3_CK_N;
output HPS_DDR3_CK_P;
output HPS_DDR3_CS_N;
output [3:0] HPS_DDR3_DM;
inout [31:0] HPS_DDR3_DQ;
inout [3:0] HPS_DDR3_DQS_N;
inout [3:0] HPS_DDR3_DQS_P;
output HPS_DDR3_ODT;
output HPS_DDR3_RAS_N;
output HPS_DDR3_RESET_N;
input HPS_DDR3_RZQ;
output HPS_DDR3_WE_N;


//*****RED/WIRE declartions

wire[31:0] hex3_hex0;
wire[15:0] hex5_hex4;

//assign HEX0 = ~hex3_hex0[6:0];
assign HEX1 = ~hex3_hex0[14:8];
assign HEX2 = ~hex3_hex0[22:16];
assign HEX3 = ~hex3_hex0[30:24];
assign HEX4 = ~hex5_hex4[6:0];
assign HEX5 = ~hex5_hex4[14:8];


assign hex3_hex0[6:0] = 7'b1110110;


 //Structural Coding
 
 
 
 mysystem u0 (
        .clk_clk            (CLOCK_50),            //         clk.clk
		  .reset_reset_n		 (1'b1), 					//reset_reset_n
        .hex3_hex0_export   (hex3_hex0),   //   hex3_hex0.export
        .hex5_hex4_export   (hex5_hex4),   //   hex5_hex4.export
        .memory_mem_a       (HPS_DDR3_ADDR),       //      memory.mem_a
        .memory_mem_ba      (HPS_DDR3_BA),      //            .mem_ba
        .memory_mem_ck      (HPS_DDR3_CK_P),      //            .mem_ck
        .memory_mem_ck_n    (HPS_DDR3_CK_N),    //            .mem_ck_n
        .memory_mem_cke     (HPS_DDR3_CKE),     //            .mem_cke
        .memory_mem_cs_n    (HPS_DDR3_CS_N),    //            .mem_cs_n
        .memory_mem_ras_n   (HPS_DDR3_RAS_N),   //            .mem_ras_n
        .memory_mem_cas_n   (HPS_DDR3_CAS_N),   //            .mem_cas_n
        .memory_mem_we_n    (HPS_DDR3_WE_N),    //            .mem_we_n
        .memory_mem_reset_n (HPS_DDR3_RESET_N), //            .mem_reset_n
        .memory_mem_dq      (HPS_DDR3_DQ),      //            .mem_dq
        .memory_mem_dqs     (HPS_DDR3_DQS_P),     //            .mem_dqs
        .memory_mem_dqs_n   (HPS_DDR3_DQS_N),   //            .mem_dqs_n
        .memory_mem_odt     (HPS_DDR3_ODT),     //            .mem_odt
        .memory_mem_dm      (HPS_DDR3_DM),      //            .mem_dm
        .memory_oct_rzqin   (HPS_DDR3_RZQ),   //            .oct_rzqin
        .pushbuttons_export (~KEY[3:0]), // pushbuttons.export
        .rled_export        (LEDR),        //        rled.export
        .switches_export    (SW)     //    switches.export
    );

endmodule
