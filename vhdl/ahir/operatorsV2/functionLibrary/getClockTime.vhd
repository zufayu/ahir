library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

library ahir;
use ahir.BaseComponents.all;


entity getClockTime is -- 
    generic (tag_length : integer);
    port ( -- 
      clock_time : out  std_logic_vector(31 downto 0);
      clk : in std_logic;
      reset : in std_logic;
      start_req : in std_logic;
      start_ack : out std_logic;
      fin_req : in std_logic;
      fin_ack   : out std_logic;
      tag_in: in std_logic_vector(tag_length-1 downto 0);
      tag_out: out std_logic_vector(tag_length-1 downto 0) -- 
    );
end entity getClockTime;

architecture Behave of getClockTime is

	type TimerState is (idle, done);
	signal tstate : TimerState;
	signal count_sig : unsigned(31 downto 0);

begin

	process(clk,reset,tstate,count_sig,start_req,fin_req,tag_in)
		variable next_state: TimerState;
		variable latch_var, decr_count, latch_otag: boolean;
	begin
		next_state := tstate;

		start_ack <= '0';
		fin_ack <= '0';

		latch_var := false;
		decr_count := false;

		case tstate is 
			when idle =>
				start_ack <= '1';
				if(start_req = '1') then
					next_state := done;
					latch_var  := true;
				end if;
			when done =>
				fin_ack <= '1';
				if(fin_req = '1') then
					next_state := idle;
				end if;
		end case;
		if(clk'event  and clk = '1') then
			if(reset = '1') then
				tstate <= idle;
				count_sig <= (others => '0');
			else
				tstate <= next_state;	
				count_sig <= count_sig + 1;
			end if;

			if(latch_var) then
				tag_out <= tag_in;
				clock_time <= std_logic_vector(count_sig);	
			end if;
		end if;
	end process;

end Behave;

