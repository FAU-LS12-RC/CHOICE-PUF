library ieee;
use ieee.std_logic_1164.all;
-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
use IEEE.NUMERIC_STD.ALL;
-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
library UNISIM;
use UNISIM.VComponents.all;

entity read_adapter is
  generic (
            -- Users to add parameters here
            DATA_WIDTH : integer	:= 32;
            -- User parameters ends
            -- Do not modify the parameters beyond this line

            -- Parameters of Axi Slave Bus Interface S00_AXI
            C_S00_AXI_DATA_WIDTH	: integer	:= 32;
            C_S00_AXI_ADDR_WIDTH	: integer	:= 4
          );
  port (
         -- Users to add ports here
         -- data in axi stream interface
         from_dma_data : in std_logic_vector(DATA_WIDTH-1 downto 0);
         from_dma_valid : in std_logic;
         from_dma_ready : out std_logic;
         
         -- data out axi stream interface       
         data_out : out std_logic_vector(DATA_WIDTH-1 downto 0);
         data_out_valid : out std_logic;
         data_out_ready : in std_logic;
         -- User ports ends
         
         -- Do not modify the ports beyond this line

         -- clk of ip 
         s00_axi_aclk	: in std_logic;
         -- rst of ip
         s00_axi_aresetn	: in std_logic;

         -- Ports of Axi Slave Bus Interface for read adapter
         s00_axi_awaddr	: in std_logic_vector(C_S00_AXI_ADDR_WIDTH-1 downto 0);
         s00_axi_awprot	: in std_logic_vector(2 downto 0);
         s00_axi_awvalid	: in std_logic;
         s00_axi_awready	: out std_logic;
         s00_axi_wdata	: in std_logic_vector(C_S00_AXI_DATA_WIDTH-1 downto 0);
         s00_axi_wstrb	: in std_logic_vector((C_S00_AXI_DATA_WIDTH/8)-1 downto 0);
         s00_axi_wvalid	: in std_logic;
         s00_axi_wready	: out std_logic;
         s00_axi_bresp	: out std_logic_vector(1 downto 0);
         s00_axi_bvalid	: out std_logic;
         s00_axi_bready	: in std_logic;
         s00_axi_araddr	: in std_logic_vector(C_S00_AXI_ADDR_WIDTH-1 downto 0);
         s00_axi_arprot	: in std_logic_vector(2 downto 0);
         s00_axi_arvalid	: in std_logic;
         s00_axi_arready	: out std_logic;
         s00_axi_rdata	: out std_logic_vector(C_S00_AXI_DATA_WIDTH-1 downto 0);
         s00_axi_rresp	: out std_logic_vector(1 downto 0);
         s00_axi_rvalid	: out std_logic;
         s00_axi_rready	: in std_logic
         
       );
   
end read_adapter;

architecture arch_imp of read_adapter is
                  
  -- component declaration
  component read_adapter_S00_AXI is
    generic (
              -- Users to add parameters here
              DATA_WIDTH : integer    := 32;
              -- User parameters ends
              -- Do not modify the parameters beyond this line
              
              C_S_AXI_DATA_WIDTH	: integer	:= 32;
              C_S_AXI_ADDR_WIDTH	: integer	:= 4
            );
    port (
           S_AXI_ACLK	: in std_logic;
           S_AXI_ARESETN	: in std_logic;
           S_AXI_AWADDR	: in std_logic_vector(C_S_AXI_ADDR_WIDTH-1 downto 0);
           S_AXI_AWPROT	: in std_logic_vector(2 downto 0);
           S_AXI_AWVALID	: in std_logic;
           S_AXI_AWREADY	: out std_logic;
           S_AXI_WDATA	: in std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0);
           S_AXI_WSTRB	: in std_logic_vector((C_S_AXI_DATA_WIDTH/8)-1 downto 0);
           S_AXI_WVALID	: in std_logic;
           S_AXI_WREADY	: out std_logic;
           S_AXI_BRESP	: out std_logic_vector(1 downto 0);
           S_AXI_BVALID	: out std_logic;
           S_AXI_BREADY	: in std_logic;
           S_AXI_ARADDR	: in std_logic_vector(C_S_AXI_ADDR_WIDTH-1 downto 0);
           S_AXI_ARPROT	: in std_logic_vector(2 downto 0);
           S_AXI_ARVALID	: in std_logic;
           S_AXI_ARREADY	: out std_logic;
           S_AXI_RDATA	: out std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0);
           S_AXI_RRESP	: out std_logic_vector(1 downto 0);
           S_AXI_RVALID	: out std_logic;
           S_AXI_RREADY	: in std_logic;

           data_out : out std_logic_vector(DATA_WIDTH-1 downto 0);
           data_out_valid : out std_logic;
           data_out_ready : in std_logic;

           from_dma_data : in std_logic_vector(DATA_WIDTH-1 downto 0);
           from_dma_valid : in std_logic;
           from_dma_ready : out std_logic

         );
  end component read_adapter_S00_AXI;
  
begin       

       -- Instantiation of Axi Bus Interface S00_AXI
       read_adapter_S00_AXI_inst : read_adapter_S00_AXI
       generic map ( 
                     DATA_WIDTH => DATA_WIDTH,
                     
                     C_S_AXI_DATA_WIDTH    => C_S00_AXI_DATA_WIDTH,
                     C_S_AXI_ADDR_WIDTH    => C_S00_AXI_ADDR_WIDTH
                   )
       port map (
                  S_AXI_ACLK    => s00_axi_aclk,
                  S_AXI_ARESETN    => s00_axi_aresetn,
                  S_AXI_AWADDR    => s00_axi_awaddr,
                  S_AXI_AWPROT    => s00_axi_awprot,
                  S_AXI_AWVALID    => s00_axi_awvalid,
                  S_AXI_AWREADY    => s00_axi_awready,
                  S_AXI_WDATA    => s00_axi_wdata,
                  S_AXI_WSTRB    => s00_axi_wstrb,
                  S_AXI_WVALID    => s00_axi_wvalid,
                  S_AXI_WREADY    => s00_axi_wready,
                  S_AXI_BRESP    => s00_axi_bresp,
                  S_AXI_BVALID    => s00_axi_bvalid,
                  S_AXI_BREADY    => s00_axi_bready,
                  S_AXI_ARADDR    => s00_axi_araddr,
                  S_AXI_ARPROT    => s00_axi_arprot,
                  S_AXI_ARVALID    => s00_axi_arvalid,
                  S_AXI_ARREADY    => s00_axi_arready,
                  S_AXI_RDATA    => s00_axi_rdata,
                  S_AXI_RRESP    => s00_axi_rresp,
                  S_AXI_RVALID    => s00_axi_rvalid,
                  S_AXI_RREADY    => s00_axi_rready,
              
                  data_out       =>  data_out,
                  data_out_valid =>  data_out_valid,
                  data_out_ready =>  data_out_ready,
                              
                  from_dma_data => from_dma_data,
                  from_dma_valid => from_dma_valid,
                  from_dma_ready => from_dma_ready
                ); 
                
        
               -- Add user logic here

               -- User logic ends

end arch_imp;