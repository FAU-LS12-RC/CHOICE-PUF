library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity pattern_ctrl_unit is
  Port ( 
        -- in ports
         clk                : in STD_LOGIC;
         payload            : in STD_LOGIC_VECTOR(32 downto 0);
         top_pattern_set    : in STD_LOGIC;
         bottom_pattern_set : in STD_LOGIC;
         fill_req           : in STD_LOGIC;
         ASR_enable         : in STD_LOGIC;
         ASR_choice_set     : in STD_LOGIC;
        -- out ports
         ASR_choice         : out STD_LOGIC_VECTOR(3 downto 0);
         pattern_ready      : out STD_LOGIC
       );
end pattern_ctrl_unit;

architecture Behavioral of pattern_ctrl_unit is

  type state_type is (s0, s1, s2);
  signal state : state_type := s0;

  signal top_pattern       : STD_LOGIC_VECTOR(31 downto 0) := x"AAAAAAAA";
  signal top_in_pattern    : STD_LOGIC := '0';
  signal bottom_pattern    : STD_LOGIC_VECTOR(31 downto 0) := x"55555555";
  signal bottom_in_pattern : STD_LOGIC := '1';

  signal count : integer range 1 to 32 := 1; 
  signal ASR_top_indx : integer range 0 to 3 := 3; 
  signal ASR_bottom_indx : integer range 0 to 3 := 0; 

begin
  main_process : process
  begin
    wait until rising_edge(clk);
    case state is
      when s0 => -- store patterns for ASR
        --pattern_ready <= '0';
        if top_pattern_set = '1' then
          top_pattern <= payload(31 downto 0);
          top_in_pattern <= payload(32);
        elsif bottom_pattern_set = '1' then
          bottom_pattern <= payload(31 downto 0);
          bottom_in_pattern <= payload(32);
        elsif ASR_choice_set = '1' then -- set choice
          ASR_top_indx <= to_integer(unsigned(payload(3 downto 2)));
          ASR_bottom_indx <= to_integer(unsigned(payload(1 downto 0)));
        elsif fill_req = '1' then
          state <= s1;
          ASR_choice <= (others => '1'); -- inactive ASRs are set to one
          ASR_choice(ASR_top_indx) <= top_pattern(0);
          ASR_choice(ASR_bottom_indx) <= bottom_pattern(0);
        end if;
      when s1 => -- define active and inactive ASRs by bit pattern
        if count < 32 then
          ASR_choice <= (others => '1'); -- inactive ASRs are set to one
          ASR_choice(ASR_top_indx) <= top_pattern(count);
          ASR_choice(ASR_bottom_indx) <= bottom_pattern(count);
          count <= count + 1;
          if count = 31 then
            pattern_ready <= '1';
          end if;
        else
          ASR_choice(ASR_top_indx) <= top_in_pattern;           -- Preparation for Request
          ASR_choice(ASR_bottom_indx) <= bottom_in_pattern;
          count <= 1;
          state <= s2;
        end if;
      when s2 => -- start puf measurement
        if ASR_enable = '1' then
          state <= s0;
          pattern_ready <= '0';
        end if;
    end case;
  end process;

end Behavioral;
