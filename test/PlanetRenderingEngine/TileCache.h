#pragma once

#include	<windows.h>
#include	<process.h>

#include	<oreore/mathlib/GraphicsMath.h>



// タイルキャッシュ上テクスチャ格納領域の使用状況を保持する構造体
struct CacheStatus
{
	//================== キャッシュ状態 =================//
	bool dirty;		// テクスチャ領域へのアクセスフラグ
	bool reserved;	// テクスチャ領域の予約フラグ

	//==== キャッシュを参照しているPageTableの情報 =====//
	short	miplv;	// PageTableのMipmapレベル
	short	mapid;	// マップ(6面ある)
	short	px;		// PageTableの座標値
	short	py;		// PageTableの座標値
};


// タイルキャッシュ（複数テクスチャを保持する一枚の巨大テクスチャ）
class TileCache
{
private:

	int	m_TexSize;		// タイルキャッシュに格納するテクスチャの解像度
	int	m_CacheWidth;	// タイルキャッシュ解像度
	int	m_CacheHeight;	// タイルキャッシュ解像度

	int	m_numMaxTiles;	// タイルキャッシュに格納できるテクスチャ最大数
	int	m_numTiles;		// タイルキャッシュに現在格納中のテクスチャ数

	unsigned int	m_TileCache;		// タイルキャッシュ（ハイトマップを格納）
	unsigned int	m_TileCache_normal;	// タイルキャッシュ（法線マップを格納）
	CacheStatus		*m_CacheStatus;		// タイルキャッシュの使用状況
	
	void InitCacheStatus();// キャッシュを初期状態（何もデータを保持しない）にする
	TileCache(){};// デフォルトコンストラクタを禁止する

	
public:

	TileCache(int cache_width, int cache_height, int tile_size);// コンストラクタ
	~TileCache();// デストラクタ
	
	inline int Reserve();													// TileCacheの未使用領域を予約する．
	inline int GetTileSize(){ return m_TexSize; }							// タイル1枚の解像度を取得する
	inline bool IsFull(){ return m_numTiles>=m_numMaxTiles ? true : false; }// タイルキャッシュが満杯かどうかチェックする

	friend class VirtualTexture;
};



// タイルキャッシュの未使用領域を予約する
inline int TileCache::Reserve()
{	
	for(int i=0; i<m_numMaxTiles; i++)
	{
		if(m_CacheStatus[i].reserved == false && m_CacheStatus[i].dirty == false)// 
		{
			m_CacheStatus[i].reserved = true;
			return i;
		}
	}

	return -1;
}