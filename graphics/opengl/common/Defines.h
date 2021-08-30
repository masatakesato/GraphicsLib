#ifndef DEFINES_H
#define	DEFINES_H


#define	MAX_MATERIALS			8

#define	MAX_DIRECTIONAL_LIGHTS	256
#define	MAX_POINT_LIGHTS		256
#define	MAX_SPOT_LIGHTS			256
#define	MAX_AREA_LIGHTS			256
//#define	MAX_LIGHTS				4096

#define	MAX_LIGHTS				512
#define	MAX_LIGHT_BUFFER		4096

#define	MAX_SHADOW_CASCADES		16
#define	MAX_SHADOW_MAPS			64
#define	SHADOWMAP_SIZE			512//2048//2048//
// 2kシャドウマップ64枚だと、GPUメモリ1GB消費.大きすぎる？

#define MAX_RENDER_QUEUE		65536
#define	MAX_VAOS				32
#define	MAX_GEOMS				32




#endif	// DEFINES_H //
