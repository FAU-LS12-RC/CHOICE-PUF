library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;
library UNISIM;
use UNISIM.VComponents.all;

entity PUF_controller is
    Port ( 
        -- in ports
        clk            : in STD_LOGIC;
        payload        : in STD_LOGIC_VECTOR(63 downto 0);
        read_valid     : in STD_LOGIC;
        write_ready    : in STD_LOGIC;
        -- out ports
        read_ready     : out STD_LOGIC;
        write_valid    : out STD_LOGIC;
        ff_reset       : out STD_LOGIC;
        ASR_enable     : out STD_LOGIC;
        ASR_tuning     : out STD_LOGIC_VECTOR(19 downto 0);
        ASR_choice     : out STD_LOGIC_VECTOR(3 downto 0)
    );
end PUF_controller;

architecture Behavioral of PUF_controller is
 
  constant PUF_REQ            :  std_logic_vector(3 downto 0):= "0001";
  constant PUF_TUNE_SET       :  std_logic_vector(3 downto 0):= "0010";
  constant PUF_CHOICE_SET     :  std_logic_vector(3 downto 0):= "0011";
  constant TOP_PATTERN_SET    :  std_logic_vector(3 downto 0):= "0100";
  constant BOTTOM_PATTERN_SET :  std_logic_vector(3 downto 0):= "0101";

   -- Internal Signals
  signal ASR_tuning_s         : STD_LOGIC_VECTOR(19 downto 0);
  signal ASR_choice_s         : STD_LOGIC_VECTOR(3 downto 0);
  signal ASR_tuning_set_s     : STD_LOGIC;
  signal ASR_choice_set_s     : STD_LOGIC;
  signal ASR_enable_s         : STD_LOGIC;
  signal read_ready_s         : STD_LOGIC;
  signal write_valid_s        : STD_LOGIC;
  signal req_s                : STD_LOGIC;
  signal ff_reset_s           : STD_LOGIC;
  signal top_pattern_set_s    : STD_LOGIC;
  signal bottom_pattern_set_s : STD_LOGIC;
  signal pattern_ready_s      : STD_LOGIC;
  signal fill_req_s           : STD_LOGIC;
    
 
    component pattern_ctrl_unit is
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
    end component;
    
    component tune_ctrl_unit is
    Port ( 
        -- in ports
        clk            : in STD_LOGIC;
        ASR_tuning_set : in STD_LOGIC;
        ASR_choice_set : in STD_LOGIC;
        payload        : in STD_LOGIC_VECTOR(9 downto 0);
        -- out ports
        ASR_tuning     : out STD_LOGIC_VECTOR(19 downto 0)
    );
    end component;
    
    component request_ctrl_unit is
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
    end component;
    
 begin
    
   req_ctrl_unit_i : request_ctrl_unit
    port map(
        clk           => clk,
        req           => req_s,
        pattern_ready => pattern_ready_s,
        ASR_enable    => ASR_enable_s,
        fill_req      => fill_req_s,
        ff_reset      => ff_reset_s,
        read_valid    => read_valid,
        read_ready    => read_ready_s,
        write_ready   => write_ready,
        write_valid   => write_valid_s
    );

    pattern_ctrl_unit_i : pattern_ctrl_unit
    port map(
        clk                => clk,
        payload            => payload(32 downto 0),
        top_pattern_set    => top_pattern_set_s,
        bottom_pattern_set => bottom_pattern_set_s,
        fill_req           => fill_req_s,
        ASR_enable         => ASR_enable_s,
        ASR_choice_set     => ASR_choice_set_s,
        ASR_choice         => ASR_choice_s,
        pattern_ready      => pattern_ready_s
    );
    
    tune_ctrl_unit_i : tune_ctrl_unit
    port map(
        clk              => clk,
        payload          => payload(9 downto 0),
        ASR_tuning_set   => ASR_tuning_set_s,
        ASR_choice_set   => ASR_choice_set_s,
        ASR_tuning       => ASR_tuning_s
    );
    
    -- Top 4 payload bits are checked for processor command interpretation
    req_s                <= '1' when payload(63 downto 60) = PUF_REQ else '0';           
    ASR_tuning_set_s     <= '1' when payload(63 downto 60) = PUF_TUNE_SET else '0';      
    ASR_choice_set_s     <= '1' when payload(63 downto 60) = PUF_CHOICE_SET else '0';    
    top_pattern_set_s    <= '1' when payload(63 downto 60) = TOP_PATTERN_SET else '0';   
    bottom_pattern_set_s <= '1' when payload(63 downto 60) = BOTTOM_PATTERN_SET else '0';

    ASR_enable           <= ASR_enable_s;
    ASR_choice           <= ASR_choice_s;
    ASR_tuning           <= ASR_tuning_s; 
    ff_reset             <= ff_reset_s; 
    read_ready           <= read_ready_s;
    write_valid          <= write_valid_s;
   
end Behavioral;
