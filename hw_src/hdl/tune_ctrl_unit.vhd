library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity tune_ctrl_unit is
    Port ( 
        -- in ports
        clk            : in STD_LOGIC;
        payload        : in STD_LOGIC_VECTOR(9 downto 0);
        ASR_tuning_set : in STD_LOGIC;
        ASR_choice_set : in STD_LOGIC;
        -- out ports
        ASR_tuning     : out STD_LOGIC_VECTOR(19 downto 0)
    );
end tune_ctrl_unit;

architecture Behavioral of tune_ctrl_unit is

  signal upper_tuning : STD_LOGIC_VECTOR(4 downto 0) := "1" & x"F"; -- upper ASR length filled with ones
  signal lower_tuning : STD_LOGIC_VECTOR(4 downto 0) := "1" & x"F"; -- lower ASR length filled with ones
    
  signal ASR_top_indx : integer range 0 to 3 := 3; 
  signal ASR_bottom_indx : integer range 0 to 3 := 0; 

begin
    main_process : process
    begin
        wait until rising_edge(clk);
        if ASR_tuning_set = '1' then
            upper_tuning <= payload(9 downto 5);
            lower_tuning <= payload(4 downto 0);
        elsif ASR_choice_set = '1'then -- we have to know which ASR combination we have to consider
            ASR_top_indx    <= to_integer(unsigned(payload(3 downto 2)));
            ASR_bottom_indx <= to_integer(unsigned(payload(1 downto 0)));
        end if;
        ASR_tuning <= (others => '1'); -- inactive ASRs are filled with ones
        ASR_tuning(ASR_top_indx*5+4 downto ASR_top_indx*5)       <= upper_tuning;
        ASR_tuning(ASR_bottom_indx*5+4 downto ASR_bottom_indx*5) <= lower_tuning;
    end process;

end Behavioral;
