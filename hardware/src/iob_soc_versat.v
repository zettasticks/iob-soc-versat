`timescale 1 ns / 1 ps

`include "bsp.vh"
`include "iob_soc_versat_conf.vh"
`include "iob_soc_versat.vh"
`include "iob_utils.vh"

//Peripherals _swreg_def.vh file includes.
`include "iob_uart_swreg_def.vh"
`include "iob_timer_swreg_def.vh"
`include "iob_versat_swreg_def.vh"

module iob_soc_versat #(

  parameter BOOTROM_ADDR_W = `IOB_SOC_VERSAT_BOOTROM_ADDR_W,
  parameter SRAM_ADDR_W = `IOB_SOC_VERSAT_SRAM_ADDR_W,
  parameter MEM_ADDR_W = `IOB_SOC_VERSAT_MEM_ADDR_W,
  parameter ADDR_W = `IOB_SOC_VERSAT_ADDR_W,
  parameter DATA_W = `IOB_SOC_VERSAT_DATA_W,
  parameter AXI_ID_W = `IOB_SOC_VERSAT_AXI_ID_W,
  parameter AXI_ADDR_W = `IOB_SOC_VERSAT_AXI_ADDR_W,
  parameter AXI_DATA_W = `IOB_SOC_VERSAT_AXI_DATA_W,
  parameter AXI_LEN_W = `IOB_SOC_VERSAT_AXI_LEN_W,
  parameter MEM_ADDR_OFFSET = `IOB_SOC_VERSAT_MEM_ADDR_OFFSET,
  parameter UART0_DATA_W = `IOB_SOC_VERSAT_UART0_DATA_W,
  parameter UART0_ADDR_W = `IOB_SOC_VERSAT_UART0_ADDR_W,
  parameter UART0_UART_DATA_W = `IOB_SOC_VERSAT_UART0_UART_DATA_W,
  parameter TIMER0_DATA_W = `IOB_SOC_VERSAT_TIMER0_DATA_W,
  parameter TIMER0_ADDR_W = `IOB_SOC_VERSAT_TIMER0_ADDR_W,
  parameter TIMER0_WDATA_W = `IOB_SOC_VERSAT_TIMER0_WDATA_W,
  parameter VERSAT0_ADDR_W = `IOB_SOC_VERSAT_VERSAT0_ADDR_W,
  parameter VERSAT0_DATA_W = `IOB_SOC_VERSAT_VERSAT0_DATA_W,
  parameter VERSAT0_MEM_ADDR_OFFSET = `IOB_SOC_VERSAT_VERSAT0_MEM_ADDR_OFFSET,
  parameter VERSAT0_AXI_ID_W = `IOB_SOC_VERSAT_VERSAT0_AXI_ID_W,
  parameter VERSAT0_AXI_LEN_W = `IOB_SOC_VERSAT_VERSAT0_AXI_LEN_W,
  parameter VERSAT0_AXI_ADDR_W = `IOB_SOC_VERSAT_VERSAT0_AXI_ADDR_W,
  parameter VERSAT0_AXI_DATA_W = `IOB_SOC_VERSAT_VERSAT0_AXI_DATA_W
) (
input clk_i,
input cke_i,
input arst_i,
output trap_o,
`ifdef IOB_SOC_VERSAT_USE_EXTMEM
output [(2*AXI_ID_W)-1:0] axi_awid_o,
output [(2*AXI_ADDR_W)-1:0] axi_awaddr_o,
output [(2*AXI_LEN_W)-1:0] axi_awlen_o,
output [(2*3)-1:0] axi_awsize_o,
output [(2*2)-1:0] axi_awburst_o,
output [(2*2)-1:0] axi_awlock_o,
output [(2*4)-1:0] axi_awcache_o,
output [(2*3)-1:0] axi_awprot_o,
output [(2*4)-1:0] axi_awqos_o,
output [(2*1)-1:0] axi_awvalid_o,
input [(2*1)-1:0] axi_awready_i,
output [(2*AXI_DATA_W)-1:0] axi_wdata_o,
output [(2*(AXI_DATA_W/8))-1:0] axi_wstrb_o,
output [(2*1)-1:0] axi_wlast_o,
output [(2*1)-1:0] axi_wvalid_o,
input [(2*1)-1:0] axi_wready_i,
input [(2*AXI_ID_W)-1:0] axi_bid_i,
input [(2*2)-1:0] axi_bresp_i,
input [(2*1)-1:0] axi_bvalid_i,
output [(2*1)-1:0] axi_bready_o,
output [(2*AXI_ID_W)-1:0] axi_arid_o,
output [(2*AXI_ADDR_W)-1:0] axi_araddr_o,
output [(2*AXI_LEN_W)-1:0] axi_arlen_o,
output [(2*3)-1:0] axi_arsize_o,
output [(2*2)-1:0] axi_arburst_o,
output [(2*2)-1:0] axi_arlock_o,
output [(2*4)-1:0] axi_arcache_o,
output [(2*3)-1:0] axi_arprot_o,
output [(2*4)-1:0] axi_arqos_o,
output [(2*1)-1:0] axi_arvalid_o,
input [(2*1)-1:0] axi_arready_i,
input [(2*AXI_ID_W)-1:0] axi_rid_i,
input [(2*AXI_DATA_W)-1:0] axi_rdata_i,
input [(2*2)-1:0] axi_rresp_i,
input [(2*1)-1:0] axi_rlast_i,
input [(2*1)-1:0] axi_rvalid_i,
output [(2*1)-1:0] axi_rready_o,
`endif
output uart_txd_o,
input uart_rxd_i,
input uart_cts_i,
output uart_rts_o 
);

   localparam integer Bbit = `IOB_SOC_VERSAT_B;
   localparam integer AddrMsb = `REQ_W - 2;


    // Internal wires for trap signals
    wire cpu_trap_o;
    assign trap_o = cpu_trap_o;

   //
   // SYSTEM RESET
   //

   wire boot;
   wire cpu_reset;

   //
   //  CPU
   //

   // instruction bus
   wire [ `REQ_W-1:0] cpu_i_req;
   wire [`RESP_W-1:0] cpu_i_resp;

   // data cat bus
   wire [ `REQ_W-1:0] cpu_d_req;
   wire [`RESP_W-1:0] cpu_d_resp;

   //instantiate the cpu
   iob_picorv32 #(
      .ADDR_W        (ADDR_W),
      .DATA_W        (DATA_W),
      .USE_COMPRESSED(`IOB_SOC_VERSAT_USE_COMPRESSED),
      .USE_MUL_DIV   (`IOB_SOC_VERSAT_USE_MUL_DIV),
`ifdef IOB_SOC_VERSAT_USE_EXTMEM
      .USE_EXTMEM    (1)
`else
      .USE_EXTMEM    (0)
`endif
   ) cpu (
      .clk_i (clk_i),
      .arst_i (cpu_reset),
      .cke_i (cke_i),
      .boot_i(boot),
      .trap_o(cpu_trap_o),

      //instruction bus
      .ibus_req_o (cpu_i_req),
      .ibus_resp_i(cpu_i_resp),

      //data bus
      .dbus_req_o (cpu_d_req),
      .dbus_resp_i(cpu_d_resp)
   );


   //
   // SPLIT CPU BUSES TO ACCESS INTERNAL OR EXTERNAL MEMORY
   //

   //internal memory instruction bus
   wire [ `REQ_W-1:0] int_mem_i_req;
   wire [`RESP_W-1:0] int_mem_i_resp;
   //external memory instruction bus
`ifdef IOB_SOC_VERSAT_USE_EXTMEM
   wire [ `REQ_W-1:0] ext_mem_i_req;
   wire [`RESP_W-1:0] ext_mem_i_resp;

   // INSTRUCTION BUS
   iob_split #(
      .ADDR_W  (ADDR_W),
      .DATA_W  (DATA_W),
      .N_SLAVES(2),
      .P_SLAVES(AddrMsb)
   ) ibus_split (
      .clk_i   (clk_i),
      .arst_i  (cpu_reset),
      // master interface
      .m_req_i (cpu_i_req),
      .m_resp_o(cpu_i_resp),
      // slaves interface
      .s_req_o ({ext_mem_i_req, int_mem_i_req}),
      .s_resp_i({ext_mem_i_resp, int_mem_i_resp})
   );
`else
   assign int_mem_i_req = cpu_i_req;
   assign cpu_i_resp    = int_mem_i_resp;
`endif


   // DATA BUS

   //internal data bus
   wire [ `REQ_W-1:0] int_d_req;
   wire [`RESP_W-1:0] int_d_resp;
`ifdef IOB_SOC_VERSAT_USE_EXTMEM
   //external memory data bus
   wire [ `REQ_W-1:0] ext_mem_d_req;
   wire [`RESP_W-1:0] ext_mem_d_resp;

   iob_split #(
      .ADDR_W  (ADDR_W),
      .DATA_W  (DATA_W),
      .N_SLAVES(2),       //E,{P,I}
      .P_SLAVES(AddrMsb)
   ) dbus_split (
      .clk_i   (clk_i),
      .arst_i  (cpu_reset),
      // master interface
      .m_req_i (cpu_d_req),
      .m_resp_o(cpu_d_resp),
      // slaves interface
      .s_req_o ({ext_mem_d_req, int_d_req}),
      .s_resp_i({ext_mem_d_resp, int_d_resp})
   );
`else
   assign int_d_req  = cpu_d_req;
   assign cpu_d_resp = int_d_resp;
`endif

   //
   // SPLIT INTERNAL MEMORY AND PERIPHERALS BUS
   //

   //slaves bus (includes internal memory + periphrals)
   wire [ (`IOB_SOC_VERSAT_N_SLAVES)*`REQ_W-1:0] slaves_req;
   wire [(`IOB_SOC_VERSAT_N_SLAVES)*`RESP_W-1:0] slaves_resp;

   iob_split #(
      .ADDR_W  (ADDR_W),
      .DATA_W  (DATA_W),
      .N_SLAVES(`IOB_SOC_VERSAT_N_SLAVES),
      .P_SLAVES(AddrMsb - 1)
   ) pbus_split (
      .clk_i   (clk_i),
      .arst_i  (cpu_reset),
      // master interface
      .m_req_i (int_d_req),
      .m_resp_o(int_d_resp),
      // slaves interface
      .s_req_o (slaves_req),
      .s_resp_i(slaves_resp)
   );


   //
   // INTERNAL SRAM MEMORY
   //

   iob_soc_versat_int_mem #(
      .ADDR_W        (ADDR_W),
      .DATA_W        (DATA_W),
      .HEXFILE       ("iob_soc_versat_firmware"),
      .BOOT_HEXFILE  ("iob_soc_versat_boot"),
      .SRAM_ADDR_W   (SRAM_ADDR_W),
      .BOOTROM_ADDR_W(BOOTROM_ADDR_W),
      .B_BIT         (`B_BIT)
   ) int_mem0 (
      .clk_i    (clk_i),
      .arst_i   (arst_i),
      .cke_i    (cke_i),
      .boot     (boot),
      .cpu_reset(cpu_reset),

      // instruction bus
      .i_req_i (int_mem_i_req),
      .i_resp_o(int_mem_i_resp),

      //data bus
      .d_req_i (slaves_req[0+:`REQ_W]),
      .d_resp_o(slaves_resp[0+:`RESP_W])
   );

`ifdef IOB_SOC_VERSAT_USE_EXTMEM
   //
   // EXTERNAL DDR MEMORY
   //

   wire [ 1+MEM_ADDR_W-2+DATA_W+DATA_W/8-1:0] ext_mem0_i_req;
   wire [1+MEM_ADDR_W+1-2+DATA_W+DATA_W/8-1:0] ext_mem0_d_req;

   assign ext_mem0_i_req = {
      ext_mem_i_req[`VALID(0)],
      ext_mem_i_req[`ADDRESS(0, MEM_ADDR_W)-2],
      ext_mem_i_req[`WRITE(0)]
   };
   assign ext_mem0_d_req = {
      ext_mem_d_req[`VALID(0)],
      ext_mem_d_req[`ADDRESS(0, MEM_ADDR_W+1)-2],
      ext_mem_d_req[`WRITE(0)]
   };

   wire [AXI_ADDR_W-1:0] internal_axi_awaddr_o;
   wire [AXI_ADDR_W-1:0] internal_axi_araddr_o;

   iob_soc_versat_ext_mem #(
      .ADDR_W     (ADDR_W),
      .DATA_W     (DATA_W),
      .FIRM_ADDR_W(MEM_ADDR_W),
      .MEM_ADDR_W (MEM_ADDR_W),
      .DDR_ADDR_W (`DDR_ADDR_W),
      .DDR_DATA_W (`DDR_DATA_W),
      .AXI_ID_W   (AXI_ID_W),
      .AXI_LEN_W  (AXI_LEN_W),
      .AXI_ADDR_W (AXI_ADDR_W),
      .AXI_DATA_W (AXI_DATA_W)
   ) ext_mem0 (
      // instruction bus
      .i_req_i (ext_mem0_i_req),
      .i_resp_o(ext_mem_i_resp),

      //data bus
      .d_req_i (ext_mem0_d_req),
      .d_resp_o(ext_mem_d_resp),

      //AXI INTERFACE
      //address write
      .axi_awid_o   (axi_awid_o[0+:AXI_ID_W]),
      .axi_awaddr_o (internal_axi_awaddr_o[0+:AXI_ADDR_W]),
      .axi_awlen_o  (axi_awlen_o[0+:AXI_LEN_W]),
      .axi_awsize_o (axi_awsize_o[0+:3]),
      .axi_awburst_o(axi_awburst_o[0+:2]),
      .axi_awlock_o (axi_awlock_o[0+:2]),
      .axi_awcache_o(axi_awcache_o[0+:4]),
      .axi_awprot_o (axi_awprot_o[0+:3]),
      .axi_awqos_o  (axi_awqos_o[0+:4]),
      .axi_awvalid_o(axi_awvalid_o[0+:1]),
      .axi_awready_i(axi_awready_i[0+:1]),
      //write
      .axi_wdata_o  (axi_wdata_o[0+:AXI_DATA_W]),
      .axi_wstrb_o  (axi_wstrb_o[0+:(AXI_DATA_W/8)]),
      .axi_wlast_o  (axi_wlast_o[0+:1]),
      .axi_wvalid_o (axi_wvalid_o[0+:1]),
      .axi_wready_i (axi_wready_i[0+:1]),
      //write response
      .axi_bid_i    (axi_bid_i[0+:AXI_ID_W]),
      .axi_bresp_i  (axi_bresp_i[0+:2]),
      .axi_bvalid_i (axi_bvalid_i[0+:1]),
      .axi_bready_o (axi_bready_o[0+:1]),
      //address read
      .axi_arid_o   (axi_arid_o[0+:AXI_ID_W]),
      .axi_araddr_o (internal_axi_araddr_o[0+:AXI_ADDR_W]),
      .axi_arlen_o  (axi_arlen_o[0+:AXI_LEN_W]),
      .axi_arsize_o (axi_arsize_o[0+:3]),
      .axi_arburst_o(axi_arburst_o[0+:2]),
      .axi_arlock_o (axi_arlock_o[0+:2]),
      .axi_arcache_o(axi_arcache_o[0+:4]),
      .axi_arprot_o (axi_arprot_o[0+:3]),
      .axi_arqos_o  (axi_arqos_o[0+:4]),
      .axi_arvalid_o(axi_arvalid_o[0+:1]),
      .axi_arready_i(axi_arready_i[0+:1]),
      //read
      .axi_rid_i    (axi_rid_i[0+:AXI_ID_W]),
      .axi_rdata_i  (axi_rdata_i[0+:AXI_DATA_W]),
      .axi_rresp_i  (axi_rresp_i[0+:2]),
      .axi_rlast_i  (axi_rlast_i[0+:1]),
      .axi_rvalid_i (axi_rvalid_i[0+:1]),
      .axi_rready_o (axi_rready_o[0+:1]),

      .clk_i (clk_i),
      .cke_i (cke_i),
      .arst_i(cpu_reset)
   );

   assign axi_awaddr_o[AXI_ADDR_W-1:0] = internal_axi_awaddr_o + MEM_ADDR_OFFSET;
   assign axi_araddr_o[AXI_ADDR_W-1:0] = internal_axi_araddr_o + MEM_ADDR_OFFSET;
`endif


   // UART0

   iob_uart
     #(
      .DATA_W(UART0_DATA_W),
      .ADDR_W(UART0_ADDR_W),
      .UART_DATA_W(UART0_UART_DATA_W)
   )
   UART0 (
      .txd_o(uart_txd_o),
      .rxd_i(uart_rxd_i),
      .cts_i(uart_cts_i),
      .rts_o(uart_rts_o),
      .clk_i(clk_i),
      .cke_i(cke_i),
      .arst_i(arst_i),
      .iob_valid_i(slaves_req[`VALID(`IOB_SOC_VERSAT_UART0)]),
      .iob_addr_i(slaves_req[`ADDRESS(`IOB_SOC_VERSAT_UART0,`IOB_UART_SWREG_ADDR_W)]),
      .iob_wdata_i(slaves_req[`WDATA(`IOB_SOC_VERSAT_UART0)]),
      .iob_wstrb_i(slaves_req[`WSTRB(`IOB_SOC_VERSAT_UART0)]),
      .iob_rvalid_o(slaves_resp[`RVALID(`IOB_SOC_VERSAT_UART0)]),
      .iob_rdata_o(slaves_resp[`RDATA(`IOB_SOC_VERSAT_UART0)]),
      .iob_ready_o(slaves_resp[`READY(`IOB_SOC_VERSAT_UART0)])
      );

   // TIMER0

   iob_timer
     #(
      .DATA_W(TIMER0_DATA_W),
      .ADDR_W(TIMER0_ADDR_W),
      .WDATA_W(TIMER0_WDATA_W)
   )
   TIMER0 (
      .iob_valid_i(slaves_req[`VALID(`IOB_SOC_VERSAT_TIMER0)]),
      .iob_addr_i(slaves_req[`ADDRESS(`IOB_SOC_VERSAT_TIMER0,`IOB_TIMER_SWREG_ADDR_W)]),
      .iob_wdata_i(slaves_req[`WDATA(`IOB_SOC_VERSAT_TIMER0)]),
      .iob_wstrb_i(slaves_req[`WSTRB(`IOB_SOC_VERSAT_TIMER0)]),
      .iob_rvalid_o(slaves_resp[`RVALID(`IOB_SOC_VERSAT_TIMER0)]),
      .iob_rdata_o(slaves_resp[`RDATA(`IOB_SOC_VERSAT_TIMER0)]),
      .iob_ready_o(slaves_resp[`READY(`IOB_SOC_VERSAT_TIMER0)]),
      .clk_i(clk_i),
      .arst_i(arst_i),
      .cke_i(cke_i)
      );

   // VERSAT0

   iob_versat
     #(
      .ADDR_W(VERSAT0_ADDR_W),
      .DATA_W(VERSAT0_DATA_W),
      .MEM_ADDR_OFFSET(VERSAT0_MEM_ADDR_OFFSET),
      .AXI_ID_W(VERSAT0_AXI_ID_W),
      .AXI_LEN_W(VERSAT0_AXI_LEN_W),
      .AXI_ADDR_W(VERSAT0_AXI_ADDR_W),
      .AXI_DATA_W(VERSAT0_AXI_DATA_W)
   )
   VERSAT0 (
      .clk_i(clk_i),
      .cke_i(cke_i),
      .arst_i(arst_i),
      .iob_valid_i(slaves_req[`VALID(`IOB_SOC_VERSAT_VERSAT0)]),
      .iob_addr_i(slaves_req[`ADDRESS(`IOB_SOC_VERSAT_VERSAT0,`IOB_VERSAT_SWREG_ADDR_W)]),
      .iob_wdata_i(slaves_req[`WDATA(`IOB_SOC_VERSAT_VERSAT0)]),
      .iob_wstrb_i(slaves_req[`WSTRB(`IOB_SOC_VERSAT_VERSAT0)]),
      .iob_rvalid_o(slaves_resp[`RVALID(`IOB_SOC_VERSAT_VERSAT0)]),
      .iob_rdata_o(slaves_resp[`RDATA(`IOB_SOC_VERSAT_VERSAT0)]),
      .iob_ready_o(slaves_resp[`READY(`IOB_SOC_VERSAT_VERSAT0)]),
      .axi_awid_o          (axi_awid_o             [1*AXI_ID_W       +:1*AXI_ID_W]),
      .axi_awaddr_o      (axi_awaddr_o           [1*AXI_ADDR_W     +:1*AXI_ADDR_W]),
      .axi_awlen_o        (axi_awlen_o            [1*AXI_LEN_W      +:1*AXI_LEN_W]),
      .axi_awsize_o      (axi_awsize_o           [1*3              +:1*3]),
      .axi_awburst_o    (axi_awburst_o          [1*2              +:1*2]),
      .axi_awlock_o      (axi_awlock_o           [1*2              +:1*2]),
      .axi_awcache_o    (axi_awcache_o          [1*4              +:1*4]),
      .axi_awprot_o      (axi_awprot_o           [1*3              +:1*3]),
      .axi_awqos_o        (axi_awqos_o            [1*4              +:1*4]),
      .axi_awvalid_o    (axi_awvalid_o          [1*1              +:1*1]),
      .axi_awready_i    (axi_awready_i          [1*1              +:1*1]),
      .axi_wdata_o        (axi_wdata_o            [1*AXI_DATA_W     +:1*AXI_DATA_W]),
      .axi_wstrb_o        (axi_wstrb_o            [1*(AXI_DATA_W/8) +:1*(AXI_DATA_W/8)]),
      .axi_wlast_o        (axi_wlast_o            [1*1              +:1*1]),
      .axi_wvalid_o      (axi_wvalid_o           [1*1              +:1*1]),
      .axi_wready_i      (axi_wready_i           [1*1              +:1*1]),
      .axi_bid_i            (axi_bid_i              [1*AXI_ID_W       +:1*AXI_ID_W]),
      .axi_bresp_i        (axi_bresp_i            [1*2              +:1*2]),
      .axi_bvalid_i      (axi_bvalid_i           [1*1              +:1*1]),
      .axi_bready_o      (axi_bready_o           [1*1              +:1*1]),
      .axi_arid_o          (axi_arid_o             [1*AXI_ID_W       +:1*AXI_ID_W]),
      .axi_araddr_o      (axi_araddr_o           [1*AXI_ADDR_W     +:1*AXI_ADDR_W]),
      .axi_arlen_o        (axi_arlen_o            [1*AXI_LEN_W      +:1*AXI_LEN_W]),
      .axi_arsize_o      (axi_arsize_o           [1*3              +:1*3]),
      .axi_arburst_o    (axi_arburst_o          [1*2              +:1*2]),
      .axi_arlock_o      (axi_arlock_o           [1*2              +:1*2]),
      .axi_arcache_o    (axi_arcache_o          [1*4              +:1*4]),
      .axi_arprot_o      (axi_arprot_o           [1*3              +:1*3]),
      .axi_arqos_o        (axi_arqos_o            [1*4              +:1*4]),
      .axi_arvalid_o    (axi_arvalid_o          [1*1              +:1*1]),
      .axi_arready_i    (axi_arready_i          [1*1              +:1*1]),
      .axi_rid_i            (axi_rid_i              [1*AXI_ID_W       +:1*AXI_ID_W]),
      .axi_rdata_i        (axi_rdata_i            [1*AXI_DATA_W     +:1*AXI_DATA_W]),
      .axi_rresp_i        (axi_rresp_i            [1*2              +:1*2]),
      .axi_rlast_i        (axi_rlast_i            [1*1              +:1*1]),
      .axi_rvalid_i      (axi_rvalid_i           [1*1              +:1*1]),
      .axi_rready_o      (axi_rready_o           [1*1              +:1*1])
      );

endmodule
