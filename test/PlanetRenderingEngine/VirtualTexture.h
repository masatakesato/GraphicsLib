#ifndef VIRTUAL_TEXTURE_H
#define VIRTUAL_TEXTURE_H

#include	<vector>
#include	<windows.h>
#include	<process.h>

#include	<oreore/thread/ThreadPool.h>
#include	<graphics/gl4x/resource/TileCacheTexture.h>

#include	"HeightmapGenerator.h"
#include	"GraphicsDevice.h"



// スレッドに渡す関数の引き数
struct ThreadParam
{
	Vec2f vtex_tc;
	int	miplv;
	int	mapid;
	Vec3f	du;
	Vec3f	dv;
	Vec3f	dn;
};



// テクスチャキャッシュのタイルと対応付いているページテーブル
struct PageTableReference
{
	short	miplv;	// PageTableのMipmapレベル
	short	mapid;	// マップ(6面ある)
	short	px;		// PageTableの座標値
	short	py;		// PageTableの座標値
};



// ページテーブルの中身
struct PageFormat
{
	unsigned char	tileid;		// タイルID（1バイト）
	unsigned char	curr_lv;	// 実際に格納しているページの解像度（1バイト）
	unsigned char	status;		// ページの状態（PAGE_STATUS_EMPTY / PAGE_STATUS_LOWRES / PAGE_STATUS_HIGHRES）（１バイト）
	bool			locked;		// タイル更新中でロックしてる（１バイト）
};

// ページテーブル構造体（1MipMapレベル分の２次元配列）
struct page_table_st
{
	short		tile_size;	// 解像度
	PageFormat	**Page;		// ページテーブルの中身
};



// ページテーブルクラス
class PageTables
{
public:

	PageTables();				// デフォルトコンストラクタ
	PageTables(int nummips);	// コンストラクタ
	PageTables(const PageTables &pt);// コピーコンストラクタ
	~PageTables();				// デストラクタ


private:

	int				m_numMIPs;	// ミップマップレベル数
	page_table_st	*m_Tables;	// ページテーブル(ミップマップレベルぶん用意する)

	friend class VirtualTexture;// VirtualTextureクラスをフレンドクラスに指定
};




// タイルキャッシュオブジェクトと，ページテーブルとを対応付けるクラス
// 機能１：タイルキャッシュの変更を，ページテーブルに反映（テクスチャ入れ替え）
// 機能２：ページテーブルの変更を，タイルキャッシュに反映（テクスチャのアップロード）
class VirtualTexture : public CWorkThread
{
public:
	
	VirtualTexture();	// デフォルトコンストラクタ
	~VirtualTexture();	// デストラクタ
	
	void AttachGraphicsDevice(GraphicsDevice *pgd);
	void InitTileCache(int cache_width, int cache_height, int tile_size);

	int RegisterPageTables(int numLv);// ページテーブルを新たに登録する

	void AttachNoiseFunc(HeightmapGenerator *noizefunc);//
	void AttachThreadPool(ThreadPool *pool);			// TileCache更新スレッドプールのポインタ取得

	unsigned int GetTileCache();
	unsigned int GetTileCache_normal();
	int GetTileSize();
	void GetCacheSize(float chachesize[4]);
	void GetTileTexture(Vec2f &page_phys_tc_high, Vec2f &page_phys_tc_low,
						int &pagelv_high, int &pagelv_low,
						float &scale_high, float &scale_low,
						const Vec3f &du, const Vec3f &dv, const Vec3f &dn,
						const Vec2f &vtex_tc, int miplv, int mapID);	// テクスチャを参照する
	void TileCache_PageOut();// タイルキャッシュの未使用データを削除する．
	void TileCache_Clear();	// タイルキャッシュを空にする
	
	void DrawPageTables();	// DrawPageTableをレンダリングする（TileCacheにロードされてる領域とか）
	void DrawTileCache();	// タイルキャッシュをレンダリングする
	

private:
	
	//===================== メンバ変数 ======================//
	TileCacheTexture	*m_TileCache;	// タイルキャッシュオブジェクト
PageTableReference	*m_TilesPageTableRefs;	// タイルテクスチャが参照するページテーブルの情報
int m_TileCapacity;

	vector <PageTables> m_TableList;	// ページテーブルのリスト
	HeightmapGenerator	*m_pNoiseFunc;	// ノイズ生成関数へのポインタ





	CRITICAL_SECTION	m_CriticalSection;	// クリティカルセクション（タイルキャッシュ/ページテーブルのデータにロックをかける）
	ThreadPool			*m_pThreadPool;	// スレッドプールへのポインタ
	ThreadParam			tParam;			// スレッド実行する関数の引数
	
	GraphicsDevice		*m_GraphicsDevice;// グラフィックスデバイス


	//================ プライベートメンバ関数 ==============//
	void TileCache_UploadTexture(int tileID, int miplv, int px, int py, int mapid, float map_dim,
								 const Vec3f &pos, const Vec3f &dv, const Vec3f &dn);// TileCacheの指定領域にテクスチャをロードする
	
	void PageTables_Update(const Vec2f &vtex_tc, const int miplv, const int mapid,
							const Vec3f &du, const Vec3f &dv, const Vec3f &dn);// ページテーブルを更新する
	void PageTables_Upsample(int miplv_curr, int px_curr, int py_curr, int mapid);		// 低解像度ページテーブルを取得する
	void PageTables_Cleanup(int px_curr, int py_curr, int miplv_curr, int tileid, int mapid);// ページテーブルを掃除する

	unsigned virtual RunProcess(void *Param);	// スレッド起動する
	void SubmitJob(ThreadPool *pool, const Vec2f &vtex_tc, int miplv, int mapid,
					const Vec3f &du, const Vec3f &dv, const Vec3f &dn);// タイルキャッシュにテクスチャをアップロードする

};



#endif // VIRTUAL_TEXTURE_H //