# Wifi Analyzer

This is a windows application that scans for nearby networks on the default network interface and shows details such as:
- SSID
- mac address
- signal strength
- channel and frequency
- network security details: authentication and encryption types, if present
- channel distribution: a chart that shows what channel each network type occupies

The network details are collected using windows' [Native Wifi API](https://docs.microsoft.com/en-us/windows/win32/api/_nwifi/)
The GUI framework is Qt.

The app is useful for evaluating wifi channel congestion/overlap and finding out network details.
