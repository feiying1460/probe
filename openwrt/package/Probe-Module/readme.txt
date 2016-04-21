在openwrt/build_dir/target-mipsel_24kec+dsp_uClibc-0.9.33.2/linux-ramips_mt7620/compat-wireless-2016-01-10/net/mac80211/rx.c
文件中加入如下：
if (unlikely(ieee80211_is_probe_req(hdr->frame_control)))
		recv_sta_probe_req(hdr->addr2);
