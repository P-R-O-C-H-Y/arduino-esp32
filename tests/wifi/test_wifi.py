def test_wifi(dut):
    dut.expect("WiFi connected")
    dut.expect("IP address:")
