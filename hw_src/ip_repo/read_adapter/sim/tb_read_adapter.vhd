----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 01/02/2018 04:16:15 PM
-- Design Name: 
-- Module Name: tb_read_adapter - Behavioral
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

library xil_defaultlib;
use xil_defaultlib.read_adapter;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity tb_read_adapter is
--  Port ( );
end tb_read_adapter;

architecture Behavioral of tb_read_adapter is


    constant clk_period : time := 2 ns;

    signal clk : std_logic := '0';
    signal rst : std_logic := '0';

    signal awaddr : std_logic_vector(3 downto 0) := (others => '0');
    signal awready : std_logic := '0';
    signal awvalid : std_logic := '0';

    signal wdata : std_logic_vector(31 downto 0);
    signal wvalid : std_logic := '0';
    signal wready : std_logic := '0';

    signal data_out : std_logic_vector(31 downto 0):= (others => '0');
    signal data_out_valid : std_logic:= '0';
    signal data_out_ready : std_logic:= '0';

    signal from_dma_data : std_logic_vector(31 downto 0):= (others => '0');
    signal from_dma_valid : std_logic:= '0';
    signal from_dma_ready : std_logic:= '0';

begin

    uut : entity xil_defaultlib.read_adapter
    port map(
                s00_axi_aclk => clk,
                s00_axi_aresetn => rst,
                s00_axi_awaddr => awaddr,
                s00_axi_awvalid => awvalid,
                s00_axi_awready => awready,
                s00_axi_wvalid => wvalid,
                s00_axi_wready => wready,
                s00_axi_wdata => wdata,
                s00_axi_wstrb => "1111",
                s00_axi_araddr => araddr,
                s00_axi_arvalid => arvalid,
                s00_axi_arready => arready,
                s00_axi_rdata => rdata,
                s00_axi_rvalid => rvalid,
                s00_axi_rready => rready,
                s00_axi_awprot => (others => '0'),
                s00_axi_bready => '1',
                s00_axi_arprot => (others => '0'), 
            
                data_out       =>  data_out,
                data_out_valid =>  data_out_valid,
                data_out_ready =>  data_out_ready,
                 
                from_dma_data  =>  from_dma_data, 
                from_dma_valid =>  from_dma_valid,
                from_dma_ready =>  from_dma_ready

            );

    uut_proc : process
    begin

        wait until rising_edge(clk);
        wait for clk_period*20;
        rst <= '1';
        wait until rising_edge(clk);

        wait until rising_edge(clk);



        awaddr <= std_logic_vector(to_unsigned(0*4,awaddr'length));
        awvalid <= '1';
        wdata <= std_logic_vector(to_unsigned(123,wdata'length));
        wvalid <= '1';
        wait until rising_edge(clk);
        wait until rising_edge(clk);
        awvalid <= '0';
        wvalid <= '0';
        wait until rising_edge(clk);

        wait until rising_edge(clk);
        wait for clk_period*20;
        wait until rising_edge(clk);
        assert false report "simulation ended" severity failure;

    end process;

    clk_proc: process
    begin
        wait for clk_period/2;
        clk <= not clk;
    end process;
    
end Behavioral;
