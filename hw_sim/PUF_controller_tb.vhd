library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity PUF_controller_tb is
--  Port ( );
end PUF_controller_tb;

architecture Behavioral of PUF_controller_tb is
    component PUF_controller is
    Port ( 
        -- in ports
        clk           : in STD_LOGIC;
        payload       : in STD_LOGIC_VECTOR(63 downto 0);
        read_valid    : in STD_LOGIC;
        write_ready   : in STD_LOGIC;
        -- out ports
        read_ready    : out STD_LOGIC;
        write_valid   : out STD_LOGIC;
        ff_reset      : out STD_LOGIC;
        ASR_enable    : out STD_LOGIC;
        ASR_tuning    : out STD_LOGIC_VECTOR(19 downto 0);
        ASR_choice    : out STD_LOGIC_VECTOR(3 downto 0)
    );
    end component;
    
    component CHOICE_PUF_gen is
    generic(
        PUF_WIDTH : integer := 32
    );
    Port ( 
        clk : in STD_LOGIC;
        ff_reset : in STD_LOGIC;
        chip_enable : in STD_LOGIC;
        ASR_length_conf : in STD_LOGIC_VECTOR(19 downto 0);
        ASR_data_conf : in STD_LOGIC_VECTOR(3 downto 0);
        -- out ports
        puf_response : out STD_LOGIC_VECTOR((PUF_WIDTH -1) downto 0)
    );
    end component;
    
    signal clk_tb : STD_LOGIC := '0';
    signal input_tb : STD_LOGIC_VECTOR(63 downto 0);
    signal read_valid_tb : STD_LOGIC := '1';
    signal write_ready_tb : STD_LOGIC := '1';
    signal read_ready_tb : STD_LOGIC;
    signal write_valid_tb : STD_LOGIC;
    signal ff_reset_tb : STD_LOGIC;
    signal ASR_enable_tb : STD_LOGIC;
    signal ASR_tuning_tb : STD_LOGIC_VECTOR(19 downto 0);
    signal ASR_choice_tb : STD_LOGIC_VECTOR(3 downto 0);
    
    signal puf_out : STD_LOGIC_VECTOR(31 downto 0);
    
    signal shiftTest3 : STD_LOGIC_VECTOR(31 downto 0) := x"0000_0000";
    signal shiftTest2 : STD_LOGIC_VECTOR(31 downto 0) := x"0000_0000";
    signal shiftTest1 : STD_LOGIC_VECTOR(31 downto 0) := x"0000_0000";
    signal shiftTest0 : STD_LOGIC_VECTOR(31 downto 0) := x"0000_0000";
    
    type state_type is (s0, s1, s2, s3, s4, s5, s6);
    signal state : state_type := s0;

begin

    ctrl : PUF_controller
    port map(
        clk => clk_tb,
        payload => input_tb,
        read_valid => read_valid_tb,
        write_ready => write_ready_tb,
        read_ready => read_ready_tb,
        write_valid => write_valid_tb,
        ff_reset => ff_reset_tb,
        ASR_enable => ASR_enable_tb,
        ASR_tuning => ASR_tuning_tb,
        ASR_choice => ASR_choice_tb
    );
    
    puf : CHOICE_PUF_gen
    generic map(
        PUF_WIDTH => 32
    )
    port map(
        clk             => clk_tb,
        chip_enable     => ASR_enable_tb,
        ff_reset        => ff_reset_tb,
        ASR_length_conf => ASR_tuning_tb,
        ASR_data_conf   => ASR_choice_tb,
        puf_response    => puf_out
    );
    
    clk_tb <= not clk_tb after 10ns;
    
    shift_test : process
    begin
        wait until rising_edge(clk_tb);
        if ASR_enable_tb = '1' then
            shiftTest3 <= std_logic_vector(shift_right(unsigned(shiftTest3), 1));
            shiftTest3(31) <= ASR_choice_tb(3);
            shiftTest2 <= std_logic_vector(shift_right(unsigned(shiftTest2), 1));
            shiftTest2(31) <= ASR_choice_tb(2);
            shiftTest1 <= std_logic_vector(shift_right(unsigned(shiftTest1), 1));
            shiftTest1(31) <= ASR_choice_tb(1);
            shiftTest0 <= std_logic_vector(shift_right(unsigned(shiftTest0), 1));
            shiftTest0(31) <= ASR_choice_tb(0);
        end if;
    end process;
    
    test_process : process
    begin
        wait until rising_edge(clk_tb);
        case state is
            when s0 =>
                input_tb <= x"4000_0002_CCCC_CCCC";
                wait for 50ns;
                state <= s1;
            when s1 =>
                input_tb <= x"5000_0001_7777_7777";
                wait for 50ns;
                state <= s2;
            when s2 =>
                input_tb <= x"3000_0000_0000_0009";
                wait for 50ns;
                state <= s3;
            when s3 =>
                input_tb <= x"2000_0000_0000_0266";
                wait for 50ns;
                state <= s4;
            when s4 =>
                input_tb <= x"1000_0000_0000_0000";
                wait for 50ns;
                state <= s5;
            when s5 =>
                input_tb <= x"0000_0000_0000_0000";
                wait for 900ns;
                state <= s5;
            when s6 =>
                input_tb <= (others => '0');
                
--                state <= s7;
--            when s7 =>
                
--                state <= s8;
--            when s8 =>
            
       end case;
    end process;
end Behavioral;
