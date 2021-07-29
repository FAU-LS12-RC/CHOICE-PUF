
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity CHOICE_PUF_gen is
  generic(
           PUF_WIDTH : integer := 32
         );
  Port ( 
         clk             : in STD_LOGIC;
         ff_reset        : in STD_LOGIC;
         chip_enable     : in STD_LOGIC;
         ASR_length_conf : in STD_LOGIC_VECTOR(19 downto 0);
         ASR_data_conf   : in STD_LOGIC_VECTOR(3 downto 0);
         puf_response    : out STD_LOGIC_VECTOR((PUF_WIDTH -1) downto 0)
       );
end CHOICE_PUF_gen;

architecture Behavioral of Choice_PUF_gen is
  component Choice_PUF is
    Port ( 
           clk             : in STD_LOGIC;
           chip_enable     : in STD_LOGIC;
           ff_reset        : in STD_LOGIC;
           ASR_length_conf : in STD_LOGIC_VECTOR(19 downto 0);
           ASR_data_conf   : in STD_LOGIC_VECTOR(3 downto 0);
           puf_bit         : out STD_LOGIC
         );
  end component;

begin
  GEN_PUF:
  for i in 0 to PUF_WIDTH-1 generate
    PUF : CHOICE_PUF port map
    (
    clk,
    chip_enable,
    ff_reset,
    ASR_length_conf,
    ASR_data_conf,
    puf_response(i)
  );
end generate GEN_PUF;

end Behavioral;
