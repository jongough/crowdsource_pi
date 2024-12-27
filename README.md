# crowdsource_pi

Contribute AIS and ARPA targets from your vessel to crowdsourcing for marine safety!

This plugin lets you upload AIS and ARPA targets (or any NMEA) to an internet server. Upload can be continuous, over intermittent internet, or scheduled, and tracks can be downsampled to fit your bandwidth.
The communication protocol is based on Apache Avro and batches track points so that the overhead for each point above timestamp and lat/lon is low, meaning it is designed to be as bandwidth conservative as possible.
