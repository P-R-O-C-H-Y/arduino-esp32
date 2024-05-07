def test_gpio(dut):
    dut.expect("Button test")
    dut.expect("Button pressed 1 times")
    dut.expect("Button pressed 2 times")
    dut.expect("Button pressed 3 times")
