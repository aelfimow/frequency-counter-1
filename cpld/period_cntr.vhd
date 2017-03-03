library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;


entity period_cntr is
	port (
		clk_a_i: in std_logic;
		clk_b_i: in std_logic;
		fc_i: in std_logic_vector(3 downto 0);
		data_o: out std_logic_vector(3 downto 0);
		base_clk_i: in std_logic;
		window_clk_o: out std_logic;
		window_clk_i: in std_logic
	);
end entity period_cntr;

architecture arch_period_cntr of period_cntr is
	signal rst: std_logic;
	signal clk: std_logic;
	signal win_clk: std_logic;
	signal div_cntr: std_logic_vector(19 downto 0);
	signal start_cnt: std_logic;
	signal freq_cntr: std_logic_vector(26 downto 0);
begin

	rst <= '1' when conv_integer(fc_i) = 15 else '0';

	div_cntr_p: process(rst, window_clk_i, win_clk)
	begin
		if (rst = '1') then
			win_clk <= '0';
			start_cnt <= '0';
			div_cntr <= (others => '0');
		elsif (window_clk_i = '1' and window_clk_i'event) then
			if (conv_integer(div_cntr) = 921599) then
				win_clk <= not win_clk;
				if (win_clk = '0') then
					start_cnt <= not start_cnt;
				end if;
				div_cntr <= (others => '0');
			else
				div_cntr <= div_cntr + 1;
			end if;
		end if;
	end process div_cntr_p;

	window_gen_clk_p: process(rst, base_clk_i)
	begin
		if (rst = '1') then
			window_clk_o <= '0';
		else
			window_clk_o <= not base_clk_i;
		end if;
	end process window_gen_clk_p;

	clk <= clk_a_i;

	freq_cntr_p: process(clk, rst, start_cnt)
	begin
		if (rst = '1') then
			freq_cntr <= (others => '0');
		elsif (clk = '1' and clk'event) then
			if (start_cnt = '1') then
				freq_cntr <= freq_cntr + 1;
			end if;
		end if;
	end process freq_cntr_p;

	output_freq_cntr_p: process(fc_i, freq_cntr, rst, start_cnt)
	begin
		data_o(0) <= rst;
		data_o(1) <= start_cnt;
		data_o(2) <= '0';
		data_o(3) <= '0';
		case (conv_integer(fc_i)) is
			when 0 => data_o <= freq_cntr(3 downto 0);
			when 1 => data_o <= freq_cntr(7 downto 4);
			when 2 => data_o <= freq_cntr(11 downto 8);
			when 3 => data_o <= freq_cntr(15 downto 12);
			when 4 => data_o <= freq_cntr(19 downto 16);
			when 5 => data_o <= freq_cntr(23 downto 20);
			when 6 => data_o(2 downto 0) <= freq_cntr(26 downto 24);
			when others => null;
		end case;
	end process output_freq_cntr_p;

end architecture arch_period_cntr;
