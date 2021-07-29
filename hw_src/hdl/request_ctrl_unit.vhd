library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity request_ctrl_unit is
    Port ( 
        -- in ports
        clk           : in STD_LOGIC;
        req           : in STD_LOGIC;
        pattern_ready : in STD_LOGIC;
        -- out ports
        ASR_enable    : out STD_LOGIC;
        fill_req      : out STD_LOGIC;
        ff_reset      : out STD_LOGIC;
        -- AXI Comm Signals
        read_valid    : in STD_LOGIC;
        read_ready    : out STD_LOGIC;
        write_ready   : in STD_LOGIC;
        write_valid   : out STD_LOGIC
    );
end request_ctrl_unit;

architecture Behavioral of request_ctrl_unit is
  type state_type is (s0, s1, s2, s3, s4);
  signal state : state_type := s0;

begin
  main_process : process
  begin
    wait until rising_edge(clk);
    case state is
      when s0 => --measurement request
        ASR_enable    <= '0';
        fill_req      <= '0';
        ff_reset      <= '0';
        read_ready    <= '1';
        write_valid   <= '1';
        if read_valid = '1' and req = '1' then
          read_ready  <= '0';
          write_valid <= '0';
          fill_req    <= '1';
          state       <=  s1;
        end if;
      when s1 => -- load pattern 
        read_ready    <= '0';
        write_valid   <= '0';
        ASR_enable    <= '1';
        fill_req      <= '0';
        if pattern_ready = '1' then
          ASR_enable  <= '0';
          state       <=  s2;
        end if;
      when s2 => -- reset ff after pattern is loaded in
        ff_reset      <= '1';
        fill_req      <= '0';
        read_ready    <= '0';
        write_valid   <= '0';
        ASR_enable    <= '0';
        state         <=  s3;
      when s3 => -- start measurement PUF
        ff_reset      <= '0';
        fill_req      <= '0';
        ASR_enable    <= '1';
        state         <=  s4;
      when s4=> -- readout PUF
        ff_reset      <= '0';
        fill_req      <= '0';
        read_ready    <= '1';
        write_valid   <= '1';
        ASR_enable    <= '0';
        state <= s0;
    end case;
  end process;

end Behavioral;
