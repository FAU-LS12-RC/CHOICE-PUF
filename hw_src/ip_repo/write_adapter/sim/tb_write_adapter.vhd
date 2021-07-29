----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 05/23/2017 04:16:15 PM
-- Design Name: 
-- Module Name: tb_write_adapter - Behavioral
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
use xil_defaultlib.write_adapter;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity tb_write_adapter is
--  Port ( );
end tb_write_adapter;

architecture Behavioral of tb_write_adapter is


    constant clk_period : time := 2 ns;

    signal clk : std_logic := '0';
    signal rst : std_logic := '0';

    signal araddr : std_logic_vector(3 downto 0) := (others => '0');
    signal arready : std_logic := '0';
    signal arvalid : std_logic := '0';


    signal awaddr : std_logic_vector(3 downto 0) := (others => '0');
    signal awready : std_logic := '0';
    signal awvalid : std_logic := '0';

    signal rdata : std_logic_vector(31 downto 0);
    signal rvalid : std_logic := '0';
    signal rready : std_logic := '1';

    signal wdata : std_logic_vector(31 downto 0);
    signal wvalid : std_logic := '0';
    signal wready : std_logic := '0';

    signal data_in : std_logic_vector(31 downto 0):= (others => '0');
    signal data_in_valid : std_logic:= '1';
    signal data_in_ready : std_logic:= '0';

    signal to_dma_data : std_logic_vector(31 downto 0):= (others => '0');
    signal to_dma_valid : std_logic:= '0';
    signal to_dma_ready : std_logic:= '0';
    signal to_dma_tlast : std_logic:= '0';

    constant MAX_PKT_LEN : integer := 10;  -- Packet length
    signal cnt_in : integer range 0 to MAX_PKT_LEN := 0; 


type stateSTIMULItype is(NEW_FILE, IDLE, SEND_DATA);
signal stateSTIMULI : stateSTIMULItype := IDLE;

begin

    uut : entity xil_defaultlib.write_adapter
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
                
                data_in        =>  data_in,
                data_in_valid  =>  data_in_valid,
                data_in_ready  =>  data_in_ready,
                             
                to_dma_data    =>  to_dma_data,
                to_dma_valid   =>  to_dma_valid,
                to_dma_tlast   =>  to_dma_tlast,   
                to_dma_ready   =>  to_dma_ready 


            );

    uut_proc : process
    begin

        wait until rising_edge(clk);
        wait for clk_period*20;
        rst <= '1';
        wait until rising_edge(clk);
        wait until rising_edge(clk);
        wait until rising_edge(clk);
        wait until rising_edge(clk);
        
        -- write reg 3 for register access
--        awaddr <= b"1000";--std_logic_vector(to_unsigned(16,awaddr'length));
--        awvalid <= '1';
--        wdata <= std_logic_vector(to_unsigned(1,wdata'length));
--        wvalid <= '1';
--        wait until rising_edge(clk);
--        wait until rising_edge(clk);
--        awvalid <= '0';
--        wvalid <= '0';
--        wait until rising_edge(clk);
        
        -- write reg 4 for DMA access
        awaddr <= b"1100";--std_logic_vector(to_unsigned(16,awaddr'length));
        awvalid <= '1';
        wdata <= std_logic_vector(to_unsigned(MAX_PKT_LEN,wdata'length));
        wvalid <= '1';
        wait until rising_edge(clk);
        wait until rising_edge(clk);
        awvalid <= '0';
        wvalid <= '0';
        to_dma_ready <= '1';
        wait until rising_edge(clk);

        --read reg 3
--        araddr <= b"1000";--std_logic_vector(to_unsigned(16,araddr'length));
--        arvalid <= '1';
--        wait until rising_edge(clk);
--        wait until rising_edge(clk);
--        wait until rising_edge(clk);
--        wait until rising_edge(clk);
--        arvalid <= '0';
--        wait until rising_edge(clk);
                
        --read reg 4
        araddr <= b"1100";--std_logic_vector(to_unsigned(16,araddr'length));
        arvalid <= '1';
        wait until rising_edge(clk);
        wait until rising_edge(clk);
        wait until rising_edge(clk);
        wait until rising_edge(clk);
        arvalid <= '0';
        wait until rising_edge(clk);

        --wait until rising_edge(clk);
        --wait for clk_period*20;
        --wait until rising_edge(clk);
        --assert false report "simulation ended" severity failure;

    end process;

-- Read data from file
read_file_pr : process

 begin
wait until rising_edge(clk);
    if (rst = '0') then
        data_in_valid <= '0';
        data_in <= x"00000000";
        cnt_in <= 0;
        stateSTIMULI <= IDLE;
    else
        case stateSTIMULI is
            when IDLE =>
                data_in_valid <= '0';
                data_in <= x"00000000";
                cnt_in <= 0;
                stateSTIMULI <= SEND_DATA;
            when SEND_DATA =>
                if(data_in_ready = '1') then
                        data_in <= std_logic_vector(to_unsigned(cnt_in,data_in'length));
                        data_in_valid <= '1'; 
                        cnt_in <= cnt_in+1;  
                end if;
                if (cnt_in = MAX_PKT_LEN-1) then
                    stateSTIMULI <= IDLE;
                end if;
            when others =>
               stateSTIMULI <= IDLE;                                       
        end case;       
   end if;
end process read_file_pr;  

    clk_proc: process
    begin
        wait for clk_period/2;
        clk <= not clk;
    end process;
    
end Behavioral;
