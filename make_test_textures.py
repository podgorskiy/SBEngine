import texture_tool as tt

tex = tt.imread('test.png')

tex = tt.utils.flip(tex)
tex = tt.generate_mipmaps(tex)

original = tex

print(tex)

tex.save_pvr('test_rgba8888.pvr')
tex.save_dds('test_rgba8888.dds')

tex = tt.transcode(tex, 'RGB888')
tex.save_pvr('test_rgb888.pvr')

tex = tt.transcode(original, 'RGB565', dither=True)
tex.save_pvr('test_rgb565.pvr')

tex = tt.transcode(original, 'ETC1', dither=True)
tex.save_pvr('test_etc1.pvr')

tex = tt.transcode(original, 'ETC2_RGB', dither=True)
tex.save_pvr('test_etc2.pvr')

# tex = tt.transcode(original, 'DXT2', dither=True)
# tex.save_pvr('test_dxt2.pvr')

tex = tt.transcode(original, 'PVRTCI_2bpp_RGB', dither=True)
tex.save_pvr('test_pvrtc_2bpp.pvr')

tex = tt.transcode(original, 'PVRTCI_4bpp_RGB', dither=True)
tex.save_pvr('test_pvrtc_4bpp.pvr')

tex = tt.utils.colour_mipmaps(original)
tex = tt.transcode(tex, 'RGB888')
tex.save_pvr('test_colored.pvr')
