`timescale 1 ns / 1 ps

`include "iob_utils.vh"

module iob_soc_versat_ext_mem #(
   parameter ADDR_W      = 0,
   parameter DATA_W      = 0,
   parameter FIRM_ADDR_W = 0,
   parameter MEM_ADDR_W  = 0,
   parameter DDR_ADDR_W  = 0,
   parameter DDR_DATA_W  = 0,
   parameter AXI_ID_W    = 0,
   parameter AXI_LEN_W   = 0,
   parameter AXI_ADDR_W  = 0,
   parameter AXI_DATA_W  = 0
) (
   // Instruction bus
   input  [1+FIRM_ADDR_W-2+`WRITE_W-1:0] i_req_i,
   output [                 `RESP_W-1:0] i_resp_o,

   // Data bus
   input  [1+1+MEM_ADDR_W-2+`WRITE_W-1:0] d_req_i,
   output [                  `RESP_W-1:0] d_resp_o,

   // AXI interface
output [AXI_ID_W-1:0] axi_awid_o,
output [AXI_ADDR_W-1:0] axi_awaddr_o,
output [AXI_LEN_W-1:0] axi_awlen_o,
output [3-1:0] axi_awsize_o,
output [2-1:0] axi_awburst_o,
output [2-1:0] axi_awlock_o,
output [4-1:0] axi_awcache_o,
output [3-1:0] axi_awprot_o,
output [4-1:0] axi_awqos_o,
output [1-1:0] axi_awvalid_o,
input [1-1:0] axi_awready_i,
output [AXI_DATA_W-1:0] axi_wdata_o,
output [(AXI_DATA_W/8)-1:0] axi_wstrb_o,
output [1-1:0] axi_wlast_o,
output [1-1:0] axi_wvalid_o,
input [1-1:0] axi_wready_i,
input [AXI_ID_W-1:0] axi_bid_i,
input [2-1:0] axi_bresp_i,
input [1-1:0] axi_bvalid_i,
output [1-1:0] axi_bready_o,
output [AXI_ID_W-1:0] axi_arid_o,
output [AXI_ADDR_W-1:0] axi_araddr_o,
output [AXI_LEN_W-1:0] axi_arlen_o,
output [3-1:0] axi_arsize_o,
output [2-1:0] axi_arburst_o,
output [2-1:0] axi_arlock_o,
output [4-1:0] axi_arcache_o,
output [3-1:0] axi_arprot_o,
output [4-1:0] axi_arqos_o,
output [1-1:0] axi_arvalid_o,
input [1-1:0] axi_arready_i,
input [AXI_ID_W-1:0] axi_rid_i,
input [AXI_DATA_W-1:0] axi_rdata_i,
input [2-1:0] axi_rresp_i,
input [1-1:0] axi_rlast_i,
input [1-1:0] axi_rvalid_i,
output [1-1:0] axi_rready_o,
input [1-1:0] clk_i,
input [1-1:0] cke_i,
input [1-1:0] arst_i);

   //
   // INSTRUCTION CACHE
   //

   // Back-end bus
   wire [1+MEM_ADDR_W+`WRITE_W-1:0] icache_be_req;
   wire [              `RESP_W-1:0] icache_be_resp;


   // Instruction cache instance
   iob_cache_iob #(
      .FE_ADDR_W    (FIRM_ADDR_W),
      .BE_ADDR_W    (MEM_ADDR_W),
      .NWAYS_W      (1),            //Number of ways
      .NLINES_W     (7),            //Cache Line Offset (number of lines)
      .WORD_OFFSET_W(3),            //Word Offset (number of words per line)
      .WTBUF_DEPTH_W(5),            //FIFO's depth -- 5 minimum for BRAM implementation
      .USE_CTRL     (0),            //Cache-Control can't be accessed
      .USE_CTRL_CNT (0)             //Remove counters
   ) icache (
      .clk_i (clk_i),
      .cke_i (cke_i),
      .arst_i(arst_i),

      // Front-end interface
      .iob_valid_i        (i_req_i[1+FIRM_ADDR_W-2+`WRITE_W-1]),
      .iob_addr_i          (i_req_i[`ADDRESS(0, FIRM_ADDR_W-2)]),
      .iob_wdata_i         (i_req_i[`WDATA(0)]),
      .iob_wstrb_i         (i_req_i[`WSTRB(0)]),
      .iob_rdata_o         (i_resp_o[`RDATA(0)]),
      .iob_rvalid_o        (i_resp_o[`RVALID(0)]),
      .iob_ready_o         (i_resp_o[`READY(0)]),
      //Control IO
      .invalidate_i (1'b0),
      .invalidate_o(),
      .wtb_empty_i  (1'b1),
      .wtb_empty_o (),
      // Back-end interface
      .be_valid_o     (icache_be_req[1+MEM_ADDR_W+`WRITE_W-1]),
      .be_addr_o       (icache_be_req[`ADDRESS(0, MEM_ADDR_W)]),
      .be_wdata_o      (icache_be_req[`WDATA(0)]),
      .be_wstrb_o      (icache_be_req[`WSTRB(0)]),
      .be_rdata_i      (icache_be_resp[`RDATA(0)]),
      .be_rvalid_i     (icache_be_resp[`RVALID(0)]),
      .be_ready_i      (icache_be_resp[`READY(0)])
   );

   //l2 cache interface signals
   wire [1+MEM_ADDR_W+`WRITE_W-1:0] l2cache_req;
   wire [`RESP_W-1:0] l2cache_resp;

   //ext_mem control signals
   wire l2_wtb_empty;
   wire invalidate;
   reg invalidate_reg;
   wire l2_valid = l2cache_req[1+MEM_ADDR_W+`WRITE_W-1];
   //Necessary logic to avoid invalidating L2 while it's being accessed by a request
   always @(posedge clk_i, posedge arst_i)
      if (arst_i) invalidate_reg <= 1'b0;
      else if (invalidate) invalidate_reg <= 1'b1;
      else if (~l2_valid) invalidate_reg <= 1'b0;
      else invalidate_reg <= invalidate_reg;

   //
   // DATA CACHE
   //

   // IOb ready and rvalid signals

   // Back-end bus
   wire [1+MEM_ADDR_W+`WRITE_W-1:0] dcache_be_req;
   wire [              `RESP_W-1:0] dcache_be_resp;

   // Data cache instance
   iob_cache_iob #(
      .FE_ADDR_W    (MEM_ADDR_W),
      .BE_ADDR_W    (MEM_ADDR_W),
      .NWAYS_W      (1),           //Number of ways
      .NLINES_W     (7),           //Cache Line Offset (number of lines)
      .WORD_OFFSET_W(3),           //Word Offset (number of words per line)
      .WTBUF_DEPTH_W(5),           //FIFO's depth -- 5 minimum for BRAM implementation
      .USE_CTRL     (1),           //Either 1 to enable cache-control or 0 to disable
      .USE_CTRL_CNT (0)            //do not change (it's implementation depends on the previous)
   ) dcache (
      .clk_i (clk_i),
      .cke_i (cke_i),
      .arst_i(arst_i),

      // Front-end interface
      .iob_valid_i        (d_req_i[2+MEM_ADDR_W-2+`WRITE_W-1]),
      .iob_addr_i          (d_req_i[`ADDRESS(0, 1+MEM_ADDR_W-2)]),
      .iob_wdata_i         (d_req_i[`WDATA(0)]),
      .iob_wstrb_i         (d_req_i[`WSTRB(0)]),
      .iob_rdata_o         (d_resp_o[`RDATA(0)]),
      .iob_rvalid_o        (d_resp_o[`RVALID(0)]),
      .iob_ready_o         (d_resp_o[`READY(0)]),
      //Control IO
      .invalidate_i (1'b0),
      .invalidate_o(invalidate),
      .wtb_empty_i  (l2_wtb_empty),
      .wtb_empty_o (),
      // Back-end interface
      .be_valid_o     (dcache_be_req[1+MEM_ADDR_W+`WRITE_W-1]),
      .be_addr_o       (dcache_be_req[`ADDRESS(0, MEM_ADDR_W)]),
      .be_wdata_o      (dcache_be_req[`WDATA(0)]),
      .be_wstrb_o      (dcache_be_req[`WSTRB(0)]),
      .be_rdata_i      (dcache_be_resp[`RDATA(0)]),
      .be_rvalid_i     (dcache_be_resp[`RVALID(0)]),
      .be_ready_i      (dcache_be_resp[`READY(0)])
   );

   // Merge cache back-ends
   iob_merge #(
      .ADDR_W   (MEM_ADDR_W),
      .N_MASTERS(2)
   ) merge_i_d_buses_into_l2 (
      .clk_i   (clk_i),
      .arst_i  (arst_i),
      // masters
      .m_req_i ({icache_be_req, dcache_be_req}),
      .m_resp_o({icache_be_resp, dcache_be_resp}),
      // slave
      .s_req_o (l2cache_req),
      .s_resp_i(l2cache_resp)
   );

   wire                  l2cache_valid;
   wire [MEM_ADDR_W-3:0] l2cache_addr;
   wire [    DATA_W-1:0] l2cache_wdata;
   wire [  DATA_W/8-1:0] l2cache_wstrb;
   wire [    DATA_W-1:0] l2cache_rdata;
   wire                  l2cache_rvalid;
   wire                  l2cache_ready;

   assign l2cache_valid            = l2cache_req[1+MEM_ADDR_W+`WRITE_W-1];
   assign l2cache_addr             = l2cache_req[`ADDRESS(0, MEM_ADDR_W)-2];
   assign l2cache_wdata            = l2cache_req[`WDATA(0)];
   assign l2cache_wstrb            = l2cache_req[`WSTRB(0)];
   assign l2cache_resp[`RDATA(0)]  = l2cache_rdata;
   assign l2cache_resp[`RVALID(0)] = l2cache_rvalid;
   assign l2cache_resp[`READY(0)]  = l2cache_ready;

   // L2 cache instance
   iob_cache_axi #(
      .AXI_ID_W     (AXI_ID_W),
      .AXI_LEN_W    (AXI_LEN_W),
      .FE_ADDR_W    (MEM_ADDR_W),
      .BE_ADDR_W    (DDR_ADDR_W),
      .BE_DATA_W    (AXI_DATA_W),
      .AXI_DATA_W   (AXI_DATA_W),
      .NWAYS_W      (2),           //Number of Ways
      .NLINES_W     (7),           //Cache Line Offset (number of lines)
      .WORD_OFFSET_W(3),           //Word Offset (number of words per line)
      .WTBUF_DEPTH_W(5),           //FIFO's depth -- 5 minimum for BRAM implementation
      .USE_CTRL     (0),           //Cache-Control can't be accessed
      .USE_CTRL_CNT (0)            //Remove counters
   ) l2cache (
      // Native interface
      .iob_valid_i        (l2cache_valid),
      .iob_addr_i          (l2cache_addr),
      .iob_wdata_i         (l2cache_wdata),
      .iob_wstrb_i         (l2cache_wstrb),
      .iob_rdata_o         (l2cache_rdata),
      .iob_rvalid_o        (l2cache_rvalid),
      .iob_ready_o         (l2cache_ready),
      //Control IO
      .invalidate_i (invalidate_reg & ~l2_valid),
      .invalidate_o(),
      .wtb_empty_i  (1'b1),
      .wtb_empty_o (l2_wtb_empty),
      // AXI interface
.axi_awid_o(axi_awid_o), //Address write channel ID.
.axi_awaddr_o(axi_awaddr_o), //Address write channel address.
.axi_awlen_o(axi_awlen_o), //Address write channel burst length.
.axi_awsize_o(axi_awsize_o), //Address write channel burst size. This signal indicates the size of each transfer in the burst.
.axi_awburst_o(axi_awburst_o), //Address write channel burst type.
.axi_awlock_o(axi_awlock_o), //Address write channel lock type.
.axi_awcache_o(axi_awcache_o), //Address write channel memory type. Set to 0000 if master output; ignored if slave input.
.axi_awprot_o(axi_awprot_o), //Address write channel protection type. Set to 000 if master output; ignored if slave input.
.axi_awqos_o(axi_awqos_o), //Address write channel quality of service.
.axi_awvalid_o(axi_awvalid_o), //Address write channel valid.
.axi_awready_i(axi_awready_i), //Address write channel ready.
.axi_wdata_o(axi_wdata_o), //Write channel data.
.axi_wstrb_o(axi_wstrb_o), //Write channel write strobe.
.axi_wlast_o(axi_wlast_o), //Write channel last word flag.
.axi_wvalid_o(axi_wvalid_o), //Write channel valid.
.axi_wready_i(axi_wready_i), //Write channel ready.
.axi_bid_i(axi_bid_i), //Write response channel ID.
.axi_bresp_i(axi_bresp_i), //Write response channel response.
.axi_bvalid_i(axi_bvalid_i), //Write response channel valid.
.axi_bready_o(axi_bready_o), //Write response channel ready.
.axi_arid_o(axi_arid_o), //Address read channel ID.
.axi_araddr_o(axi_araddr_o), //Address read channel address.
.axi_arlen_o(axi_arlen_o), //Address read channel burst length.
.axi_arsize_o(axi_arsize_o), //Address read channel burst size. This signal indicates the size of each transfer in the burst.
.axi_arburst_o(axi_arburst_o), //Address read channel burst type.
.axi_arlock_o(axi_arlock_o), //Address read channel lock type.
.axi_arcache_o(axi_arcache_o), //Address read channel memory type. Set to 0000 if master output; ignored if slave input.
.axi_arprot_o(axi_arprot_o), //Address read channel protection type. Set to 000 if master output; ignored if slave input.
.axi_arqos_o(axi_arqos_o), //Address read channel quality of service.
.axi_arvalid_o(axi_arvalid_o), //Address read channel valid.
.axi_arready_i(axi_arready_i), //Address read channel ready.
.axi_rid_i(axi_rid_i), //Read channel ID.
.axi_rdata_i(axi_rdata_i), //Read channel data.
.axi_rresp_i(axi_rresp_i), //Read channel response.
.axi_rlast_i(axi_rlast_i), //Read channel last word.
.axi_rvalid_i(axi_rvalid_i), //Read channel valid.
.axi_rready_o(axi_rready_o), //Read channel ready.
      .clk_i         (clk_i),
      .cke_i         (cke_i),
      .arst_i        (arst_i)
   );

endmodule
