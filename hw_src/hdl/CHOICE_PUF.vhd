
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;
library UNISIM;
use UNISIM.VComponents.all;

entity CHOICE_PUF is
  Port ( 
         clk                  : in STD_LOGIC;
         chip_enable          : in STD_LOGIC;
         ff_reset             : in STD_LOGIC;
         ASR_length_conf      : in STD_LOGIC_VECTOR(19 downto 0);
         ASR_data_conf        : in STD_LOGIC_VECTOR(3 downto 0);
         puf_bit              : out STD_LOGIC
       );
end CHOICE_PUF;

architecture Behavioral of CHOICE_PUF is

  -- PUF signals
  signal carry_out : STD_LOGIC;
  signal carry_dummy : STD_LOGIC_VECTOR(2 downto 0);
  signal ASRQ0, ASRQ1, ASRQ2, ASRQ3 : STD_LOGIC;

  -- Attributes
  attribute DONT_TOUCH : string;
  attribute DONT_TOUCH of  CARRY4_inst : label is "TRUE";


begin
  SRLC32E_inst_3 : SRLC32E
  generic map (
                INIT => X"00000000"
              )
  port map (
             Q   => ASRQ3, -- SRL data output
             Q31 => open, -- SRL cascade output pin
             A   => ASR_length_conf(19 downto 15), -- 5-bit shift length select input
             CE  => chip_enable, -- Chip enable input
             CLK => clk, -- Clock input
             D   => ASR_data_conf(3) -- SRL data input
           );

  SRLC32E_inst_2 : SRLC32E
  generic map (
                INIT => X"00000000"
              )
  port map (
             Q   => ASRQ2, -- SRL data output
             Q31 => open, -- SRL cascade output pin
             A   => ASR_length_conf(14 downto 10), -- 5-bit shift length select input
             CE  => chip_enable, -- Chip enable input
             CLK => clk, -- Clock input
             D   => ASR_data_conf(2) -- SRL data input
           );

  SRLC32E_inst_1 : SRLC32E
  generic map (
                INIT => X"00000000"
              )
  port map (
             Q   => ASRQ1, -- SRL data output
             Q31 => open, -- SRL cascade output pin
             A   => ASR_length_conf(9 downto 5), -- 5-bit shift length select input
             CE  => chip_enable, -- Chip enable input
             CLK => clk, -- Clock input
             D   => ASR_data_conf(1) -- SRL data input
           );

  SRLC32E_inst_0 : SRLC32E
  generic map (
                INIT => X"00000000"
              )
  port map (
             Q   => ASRQ0, -- SRL data output
             Q31 => open, -- SRL cascade output pin
             A   => ASR_length_conf(4 downto 0), -- 5-bit shift length select input
             CE  => chip_enable, -- Chip enable input
             CLK => clk, -- Clock input
             D   => ASR_data_conf(0) -- SRL data input
           );



  CARRY4_inst : CARRY4 -- the "top" carry chain
  port map (
      CO(3)          => carry_out,
      CO(2 downto 0) => carry_dummy,
      O              => open,   -- 4-bit carry chain XOR data out
      CI             => '1',    -- 1-bit carry cascade input (prev CARRY_BW)
      CYINIT         => '1',    -- 1-bit carry initialization (prev 0)
      DI             => "0000", -- 4-bit carry-MUX data in
      S(3)           => ASRQ3,
      S(2)           => ASRQ2,
      S(1)           => ASRQ1,
      S(0)           => ASRQ0
);

FDCPE_inst : FDPE
generic map (
              INIT => '0'
            ) 
port map (
           Q   => puf_bit,     -- Data output
           C   => clk,         -- Clock input
           CE  => ff_reset,    -- Clock enable input
           D   => '0',         -- Data input
           PRE => carry_out -- Asynchronous set input
         );


end Behavioral;
