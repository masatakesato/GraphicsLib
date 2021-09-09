#ifndef	TILE_CACHE_TEXTURE_H
#define	TILE_CACHE_TEXTURE_H

#include	<windows.h>
#include	<process.h>

#include	<oreore/mathlib/GraphicsMath.h>

#include	"CacheDataManager.h"
#include	"Texture.h"
using namespace OreOreLib;


// タイルキャッシュテクスチャ格納領域の使用状況を保持する構造体
struct TileStatus
{
	//================== キャッシュ状態 =================//
	bool dirty;		// テクスチャ領域へのアクセスフラグ
	bool reserved;	// テクスチャ領域の予約フラグ
};


// タイルキャッシュ（複数テクスチャを保持する一枚の巨大テクスチャ）
class TileCacheTexture
{
public:

	TileCacheTexture( int cache_width, int cache_height, int tile_size );// コンストラクタ
	~TileCacheTexture();// デストラクタ
	
	Texture2D *GetTexture() const		{ return (Texture2D *)&m_TileCache; }
uint32 GetTextureID() const			{ return m_TileCache.texID; }
uint32 GetTextureIDNormal() const			{ return m_TileCache_normal.texID; }

	int GetTexSize() const				{ return m_TexSize; }
	int GetTileCapacity() const			{ return m_CacheManager.numMaxSlots(); }
	int GetCacheWidth() const			{ return m_CacheWidth; }
	int GetCacheHeight() const			{ return m_CacheHeight; }
	
	bool IsRererved( int tile_id) const	{ return m_CacheManager.IsRererved(tile_id); }
	bool IsDirty( int tile_id ) const	{ return m_CacheManager.IsDirty(tile_id); }	// 最近データへのアクセスがあったかどうか調べる
	bool IsClean( int tile_id ) const	{ return m_CacheManager.IsClean(tile_id); }// アクセスなしの放置状態になっているかどうか調べる

	inline bool SetClean( int tile_id );
	inline bool SetDirty( int tile_id );
	inline int Reserve();													// TileCacheの未使用領域を見つけて予約する．
	//inline bool Reserve( int tile_id );										// 指定したタイルを予約する
	inline bool Free( int tile_id );										// 指定したタイルを未使用状態にする
	inline bool FreeAll();													// 全てのタイルを未使用状態にする

	inline int GetTileSize(){ return m_TexSize; }							// タイル1枚の解像度を取得する
	inline bool IsFull(){ return m_CacheManager.IsFull(); }// タイルキャッシュが満杯かどうかチェックする


private:

	int	m_TexSize;		// タイルキャッシュに格納するテクスチャの解像度
	int	m_CacheWidth;	// タイルキャッシュ解像度
	int	m_CacheHeight;	// タイルキャッシュ解像度

//	int	m_numMaxTiles;	// タイルキャッシュに格納できるテクスチャ最大数
//	int	m_numTiles;		// タイルキャッシュに現在格納中のテクスチャ数

	Texture2D		m_TileCache;//unsigned int	m_TileCache;		// タイルキャッシュ（ハイトマップを格納）
	Texture2D		m_TileCache_normal;//unsigned int	m_TileCache_normal;	// タイルキャッシュ（法線マップを格納）

	CacheDataManager	m_CacheManager;
	//TileStatus		*m_CacheStatus;		// タイルキャッシュの使用状況
	
	//void InitCacheStatus();// キャッシュを初期状態（何もデータを保持しない）にする
	TileCacheTexture(){};// デフォルトコンストラクタを禁止する
};



inline bool TileCacheTexture::SetClean( int tile_id )
{
	return m_CacheManager.SetClean( tile_id );
	//(m_CacheStatus + tile_id)->dirty	= false;
	//return true;
}


inline bool TileCacheTexture::SetDirty( int tile_id )
{
	return m_CacheManager.SetDirty( tile_id );
	/*
	TileStatus *pstatus = m_CacheStatus + tile_id;

	if( !pstatus->reserved )	return false;
	pstatus->dirty	= true;
	
	return true;
	*/
}




// タイルキャッシュの未使用領域を探して予約する
inline int TileCacheTexture::Reserve()
{
	return m_CacheManager.ReserveSlot();
	//for( int i=0; i<m_numMaxTiles; ++i )
	//{
	//	TileStatus *pstatus = m_CacheStatus + i;
	//	if( pstatus->reserved == false /*&& pstatus->dirty == false*/ )// 
	//	{
	//		pstatus->reserved = true;
	//		pstatus->dirty	= true;
	//		m_numTiles++;

	//		return i;
	//	}
	//}

	//return -1;
}


// 指定したタイルを予約する
//inline bool TileCacheTexture::Reserve( int tile_id )
//{
//	TileStatus *pstatus = m_CacheStatus + tile_id;
//
//	if( pstatus->reserved )	return false;
//
//	pstatus->reserved	= true; // タイル領域を予約する
//	pstatus->dirty		= true;	// アップロード直後はアクセス痕跡を残す
//	m_numTiles++;
//
//	return true;
//}


// 指定したタイルを未使用状態にする
inline bool TileCacheTexture::Free( int tile_id )
{
	return m_CacheManager.FreeSlot( tile_id );
	//TileStatus *pstatus = m_CacheStatus + tile_id;

	//pstatus->reserved	= false; // タイル領域を予約する
	//pstatus->dirty		= false;	// アップロード直後はアクセス痕跡を残す
	//m_numTiles--;

	//return true;
}


// 全てのタイルを未使用状態にする
inline bool TileCacheTexture::FreeAll()
{
	m_CacheManager.Clear();

	//for( int i=0; i<m_numMaxTiles; ++i )
	//{
	//	TileStatus *pstatus = m_CacheStatus + i;
	//	pstatus->reserved	= false;
	//	pstatus->dirty		= false;
	//}

	//m_numTiles	= 0;

	return true;
}



#endif	// TILE_CACHE_TEXTURE_H //