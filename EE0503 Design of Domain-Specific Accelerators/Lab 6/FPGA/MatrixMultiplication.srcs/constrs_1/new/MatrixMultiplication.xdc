set_property -dict { PACKAGE_PIN W19 IOSTANDARD LVCMOS33 } [get_ports clock];
create_clock -name sys_clk_pin -period 20.00 [get_ports clock]

set_property -dict { PACKAGE_PIN V17 IOSTANDARD LVCMOS33 } [get_ports start];
set_property -dict { PACKAGE_PIN W17 IOSTANDARD LVCMOS33 } [get_ports check];
set_property -dict { PACKAGE_PIN N15 IOSTANDARD LVCMOS33 } [get_ports reset];

set_property -dict { PACKAGE_PIN M21 IOSTANDARD LVCMOS33 } [get_ports finish];
set_property -dict { PACKAGE_PIN L21 IOSTANDARD LVCMOS33 } [get_ports correct];
