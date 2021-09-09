#include	"VirtualTexture.h"

#include	<math.h>
#include	<iostream>
using namespace std;




#define		PAGE_STATUS_EMPTY		0	// タイルキャッシュへのリンクが空
#define		PAGE_STATUS_LOWRES		1	// アップロード済み
#define		PAGE_STATUS_HIGHRES		2	// アップロード済み

#define		MAX_MIPLV	12


//######################################## PageTable ######################################//
PageTables::PageTables()
{
tcout << _T("PageTables::PageTables()...") << tendl;

	m_numMIPs	= 0;
	m_Tables	= NULL;
}


// ページテーブルを初期化する
PageTables::PageTables(int nummips)
{
tcout << _T("PageTables::PageTables(int nummips)...") << tendl;

	int i;
	int x, y;

	m_numMIPs	= min(nummips, MAX_MIPLV);
	m_Tables	= new page_table_st[m_numMIPs];
	
		
	// 各ミップマップレベルのテーブルを初期化する
	for(i=0; i<m_numMIPs; i++)
	{
		m_Tables[i].tile_size	= (int)pow(2.0, i);
		m_Tables[i].Page		= new PageFormat*[ m_Tables[i].tile_size ];

		for(x=0; x<m_Tables[i].tile_size; x++)
		{
			m_Tables[i].Page[x] = new PageFormat[m_Tables[i].tile_size];

			for(y=0; y<m_Tables[i].tile_size; y++)
			{
				m_Tables[i].Page[x][y].tileid	= 0;
				m_Tables[i].Page[x][y].curr_lv	= 0;
				m_Tables[i].Page[x][y].status	= PAGE_STATUS_EMPTY;
				m_Tables[i].Page[x][y].locked	= false;
			}// end of y loop
		}// end of x loop

	}// end of j loop

}


// コピーコンストラクタ
PageTables::PageTables(const PageTables &pt)
{
tcout << _T("PageTables::PageTables(const PageTables &pt)...") << tendl;
	int i, x, y;

	m_numMIPs	= pt.m_numMIPs;
	m_Tables	= new page_table_st[m_numMIPs];

	// 各ミップマップレベルのテーブルを初期化する
	for(i=0; i<m_numMIPs; i++)
	{
		m_Tables[i].tile_size	= pt.m_Tables[i].tile_size;					// tile_sizeをコピー
		m_Tables[i].Page		= new PageFormat*[ m_Tables[i].tile_size ];	// 
		
		// **Pageをコピー
		for(x=0; x<m_Tables[i].tile_size; x++)
		{
			m_Tables[i].Page[x] = new PageFormat[m_Tables[i].tile_size];

			for(y=0; y<m_Tables[i].tile_size; y++)
			{
				m_Tables[i].Page[x][y].tileid	= pt.m_Tables[i].Page[x][y].tileid;
				m_Tables[i].Page[x][y].curr_lv	= pt.m_Tables[i].Page[x][y].curr_lv;
				m_Tables[i].Page[x][y].status	= pt.m_Tables[i].Page[x][y].status;
				m_Tables[i].Page[x][y].locked	= pt.m_Tables[i].Page[x][y].locked;
			}// end of y loop
		}// end of x loop

	}// end of j loop

}




// デストラクタ
PageTables::~PageTables()
{
tcout << _T("PageTables::~PageTables()...") << tendl;
	
	if(m_Tables)
	{
		int i, x;
		// page_table_stのPageを解放する
		for(i=0; i<m_numMIPs; i++)
		{
			if(m_Tables[i].Page)
			{
				for(x=0; x<m_Tables[i].tile_size; x++)
					delete [] m_Tables[i].Page[x];
				delete [] m_Tables[i].Page;

				m_Tables[i].Page = NULL;
			}
		}

		// ページテーブル配列を解放する
		delete [] m_Tables;
		m_Tables = NULL;
	}
}



//######################################### VitrualTexture ###########################################//

VirtualTexture::VirtualTexture()
{
tcout << _T("VirtualTexture::VirtualTexture()...") << tendl;

	m_TileCache		= NULL;
m_TilesPageTableRefs	= NULL;
m_TileCapacity	= 0;
	m_pNoiseFunc	= NULL;

	InitializeCriticalSection(&m_CriticalSection);// クリティカルセクションの初期化
}


VirtualTexture::~VirtualTexture()
{
tcout << _T("VirtualTexture::~VirtualTexture()...") << tendl;

	m_pNoiseFunc		= NULL;// ノイズ生成オブジェクトへの参照を無効化
	m_GraphicsDevice	= NULL;// グラフィックスデバイスオブジェクトへの参照無効化
	m_pThreadPool		= NULL;// スレッドプールオブジェクトへの参照無効化

	SafeDelete( m_TileCache );// タイルキャッシュ削除	
	SafeDeleteArray( m_TilesPageTableRefs );// タイルキャッシュが参照するページテーブル情報の削除

	m_TableList.clear();// 各要素(PageTablesオブジェクト)のデストラクタが呼ばれる？
}



void VirtualTexture::AttachGraphicsDevice(GraphicsDevice *pgd)
{
tcout << _T("VirtualTexture::AttachGraphicsDevice()...") << tendl;

	m_GraphicsDevice = pgd;
}



void VirtualTexture::InitTileCache( int cache_width, int cache_height, int tile_size )
{
	EnterCriticalSection(&m_CriticalSection);
tcout << _T("VirtualTexture::InitTileCache()...") << tendl;

	m_GraphicsDevice->Begin();

	SafeDelete( m_TileCache );
	m_TileCache = new TileCacheTexture( cache_width, cache_height, tile_size );
		
	// タイルキャッシュが参照するページテーブル情報
	m_TileCapacity	= m_TileCache->GetTileCapacity();
	m_TilesPageTableRefs	= new PageTableReference[ m_TileCapacity ];

	for( int i=0; i<m_TileCapacity;  ++i )
	{
		m_TilesPageTableRefs[i].mapid	= -1;
		m_TilesPageTableRefs[i].miplv	= -1;	// MIPMAPレベル（登録してないタイルのレベルはゼロ）
		m_TilesPageTableRefs[i].px		= -1;
		m_TilesPageTableRefs[i].py		= -1;
	}// end of i loop



	m_GraphicsDevice->End();

	LeaveCriticalSection(&m_CriticalSection);
}


// ページテーブルを追加する
int VirtualTexture::RegisterPageTables(int numLv)
{
tcout << _T("VirtualTexture::RegisterPageTables()...") << tendl;

	m_TableList.push_back( PageTables(numLv) );
	return	m_TableList.size() -1;
}


void VirtualTexture::AttachNoiseFunc(HeightmapGenerator *noisefunc)
{
	m_pNoiseFunc = noisefunc;
}

void VirtualTexture::AttachThreadPool(ThreadPool *pool)
{
	m_pThreadPool = pool;
}


unsigned int VirtualTexture::GetTileCache()
{
	return m_TileCache->GetTextureID();//m_TileCache->m_TileCache.texID;
}

unsigned int VirtualTexture::GetTileCache_normal()
{
	return m_TileCache->GetTextureIDNormal();//m_TileCache->m_TileCache_normal.texID;
}


int VirtualTexture::GetTileSize()
{
	return	m_TileCache->GetTexSize();//m_TileCache->m_TexSize;
}

// タイルキャッシュのサイズを取得する
void VirtualTexture::GetCacheSize( float cachesize[4] )
{	
	cachesize[0] = (float)m_TileCache->GetCacheWidth();//m_CacheWidth;
	cachesize[1] = (float)m_TileCache->GetCacheHeight();//m_CacheHeight;
	cachesize[2] = (float)1.0f / cachesize[0];
	cachesize[3] = (float)1.0f / cachesize[1];
}



void VirtualTexture::GetTileTexture(Vec2f &page_phys_tc_high, Vec2f &page_phys_tc_low,
									int &pagelv_high, int &pagelv_low,
									float &scale_high, float &scale_low,
									const Vec3f &du, const Vec3f &dv, const Vec3f &dn,
									const Vec2f &vtex_tc, int tile_miplv, int mapID)
{
	page_table_st	*pHigher, *pLower;
	int		miplv_high, px_high, py_high;
	int		miplv_low, px_low, py_low;

	InitVec( page_phys_tc_high, 0.f, 0.f );
	InitVec( page_phys_tc_low, 0.f, 0.f );
	miplv_high	= max(min(tile_miplv, m_TableList[mapID].m_numMIPs-1), 0);
	miplv_low	= max(min(tile_miplv-1, m_TableList[mapID].m_numMIPs-1), 0);
	
	
	//=========== 参照先ページテーブルの状態を取得する =========//

	// 低解像度テクスチャのページテーブルを取得
	pLower = &m_TableList[mapID].m_Tables[miplv_low];//&m_PageTables[miplv_low];// 0以上かつtile_miplv-1以下のMIPMAP解像度で最大のもの
	px_low = (int)floor( vtex_tc.x * (float)(pLower->tile_size) );// タイルキャッシュ上のピクセル位置
	py_low = (int)floor( vtex_tc.y * (float)(pLower->tile_size) );// タイルキャッシュ上のピクセル位置
	pagelv_low = pLower->Page[px_low][py_low].curr_lv;

	if(pagelv_low < miplv_low)
	{
		PageTables_Upsample(miplv_low, px_low, py_low, mapID); // とりあえずテクスチャができてるレベルまで呼び出す
		pagelv_low = pLower->Page[px_low][py_low].curr_lv;
	}
	scale_low = pow(2.0f, pagelv_low-tile_miplv);
	page_phys_tc_low.x = float(pLower->Page[px_low][py_low].tileid % m_TileCache->GetCacheWidth() );//m_TileCache->m_CacheWidth);
	page_phys_tc_low.y = float(pLower->Page[px_low][py_low].tileid / m_TileCache->GetCacheWidth() );//m_TileCache->m_CacheWidth);
	m_TileCache->SetDirty( pLower->Page[px_low][py_low].tileid );	//m_TileCache->m_CacheStatus[pLower->Page[px_low][py_low].tileid].dirty = true;

	// 高解像度テクスチャのページテーブルを取得
	pHigher = &m_TableList[mapID].m_Tables[miplv_high];
	px_high = (int)floor( vtex_tc.x * (float)(pHigher->tile_size) );
	py_high = (int)floor( vtex_tc.y * (float)(pHigher->tile_size) );
	pagelv_high = pHigher->Page[px_high][py_high].curr_lv;

	if(pagelv_high < miplv_high)
	{
		pagelv_high = pagelv_low;
		scale_high = scale_low;
		page_phys_tc_high = page_phys_tc_low;
	}
	else
	{
		scale_high = pow(2.0f, pagelv_high-tile_miplv);
		page_phys_tc_high.x = float(pHigher->Page[px_high][py_high].tileid % m_TileCache->GetCacheWidth() );//m_TileCache->m_CacheWidth);
		page_phys_tc_high.y = float(pHigher->Page[px_high][py_high].tileid / m_TileCache->GetCacheWidth() );//m_TileCache->m_CacheWidth);
		m_TileCache->SetDirty( pHigher->Page[px_high][py_high].tileid );//m_TileCache->m_CacheStatus[pHigher->Page[px_high][py_high].tileid].dirty = true;
	}
	
	if(	pHigher->Page[px_high][py_high].status == PAGE_STATUS_HIGHRES || pHigher->Page[px_high][py_high].locked == true)
		return;

	//===== 低解像度テクスチャ時で，タイル領域がロックされていなかったらスレッド実行する ====// →”ページテーブルのロック”と”スレッド実行”の間に処理が割り込んでる？
	SubmitJob(m_pThreadPool, vtex_tc, miplv_high, mapID, du, dv, dn);

}









// TileCacheにテクスチャをアップロードする
void VirtualTexture::TileCache_UploadTexture( int tileID, int miplv, int px, int py, int mapid, float map_dim,
											const Vec3f &pos, const Vec3f &dv, const Vec3f &dn )
{
	int xoffset = tileID % m_TileCache->GetCacheWidth() * m_TileCache->GetTexSize();
	int yoffset = tileID / m_TileCache->GetCacheWidth() * m_TileCache->GetTexSize();

	//======== テクスチャを生成してタイルキャッシュにアップロードする ========//
	m_GraphicsDevice->Begin();
	m_pNoiseFunc->Draw(	m_TileCache->GetTextureID()/*m_TileCache.texID*/, m_TileCache->GetTextureIDNormal()/*m_TileCache_normal.texID*/, xoffset, yoffset, map_dim, pos, dn, dv);
	m_GraphicsDevice->End();

	//============== タイルキャッシュのステータスを更新する =================//
	m_TileCache->SetDirty( tileID );
//	if( m_TileCache->IsRererved(tileID) )	m_TileCache->Free(tileID);
//	m_TileCache->Reserve( tileID );
	//m_TileCache->m_CacheStatus[tileID].reserved = true; // タイル領域を予約する
	//m_TileCache->m_CacheStatus[tileID].dirty = true;	// アップロード直後はアクセス痕跡を残す
	//m_TileCache->m_numTiles++;

	//====== タイルキャッシュに対応するページテーブルの情報を更新する =======//
	m_TilesPageTableRefs[tileID].miplv	= miplv;
	m_TilesPageTableRefs[tileID].mapid	= mapid;
	m_TilesPageTableRefs[tileID].px		= px;
	m_TilesPageTableRefs[tileID].py		= py;
}


// miplv階層に達するまでTileCacheにテクスチャを順次アップロードする
void VirtualTexture::PageTables_Update(const Vec2f &vtex_tc, const int miplv, const int mapid,
										const Vec3f &du, const Vec3f &dv, const Vec3f &dn)
{
	int			i;
	int			px, py;
	float		depth, scale, map_dim;	// カメラ奥行き, スケール
	Vec3f		pos_curr;
	page_table_st	*pCurr;
	int	tileID;

	//tcout << _T("TileCache_Update...begin") << tendl;

	map_dim = Length(du);// タイル一辺の大きさ

	for(i=0; i<=miplv; i++)// 解像度の低いPageTableから順次テクスチャをアップロードする
	{
		pCurr = &m_TableList[mapid].m_Tables[i];
		px = (int)floor(vtex_tc.x * (float)(pCurr->tile_size) );
		py = (int)floor(vtex_tc.y * (float)(pCurr->tile_size) );

		pCurr->Page[px][py].locked = true; // ページテーブルをロックする
		m_TileCache->SetDirty( pCurr->Page[px][py].tileid );//	m_TileCache->m_CacheStatus[ pCurr->Page[px][py].tileid ].dirty = true;// キャッシュに足あとを残す
		
		//=============== 既にタイルをアップロード済なら次のMIPMAPレベルにスキップ =============//
		if( pCurr->Page[px][py].status == PAGE_STATUS_HIGHRES )
		{
			pCurr->Page[px][py].locked = false;
			continue;
		}
		
		//========= TileCacheにテクスチャをアップロードし，TileCacheステータスを更新する =======//
		if( (tileID=m_TileCache->Reserve()) == -1)// キャッシュ領域の取得に失敗
		{	
			PageTables_Upsample(i, px, py, mapid);
			return;
		}
		else// キャッシュ領域の取得に成功
		{
			scale	= pow(2.0f, float(-i));
			depth	= 0.5f * map_dim * (1.0f - scale);// 視点の奥行き方向移動量

			// タイル左下頂点の位置. scale * (float(px) + 0.5f)：解像度レベルiにおけるタイル中心座標，- 0.5f:タイル中心に移動
			float coeff_u = scale*(float(px) + 0.5f) - 0.5f;
			float coeff_v = scale*(float(py) + 0.5f) - 0.5f;
			AddScaled(pos_curr, coeff_u, du, coeff_v, dv);

			// 法線方向にカメラ移動
			AddScaled(pos_curr, pos_curr, depth, dn);
			
			// テクスチャアップロード + PageTagbes情報を更新
			TileCache_UploadTexture( tileID, i, px, py, mapid, map_dim, pos_curr, dv, dn );
			
			pCurr->Page[px][py].tileid = tileID;					// TileCacheのIDを登録する
			pCurr->Page[px][py].curr_lv = i;
			pCurr->Page[px][py].status = PAGE_STATUS_HIGHRES;// リンク先タイルの状態を"高解像度"にする
			pCurr->Page[px][py].locked = false;				// ページテーブルのロックを解除する
		}
		
	}// end of i loop

	//tcout << _T("TileCache_Update...end") << tendl;
}


// タイルキャッシュ上の未使用データを削除する
void VirtualTexture::TileCache_PageOut()
{
	int i;
	
	EnterCriticalSection( &m_CriticalSection );// TileCache(とPageTable)の更新処理が割り込むのを防止するクリティカルセクション
	
	if(m_TileCache->IsFull()==false) goto Check_Dirty;

	//============ タイルキャッシュ上で長期間参照されていないタイル領域を解放する ==========//
	for(i=0; i<m_TileCache->GetTileCapacity(); ++i )//m_numMaxTiles; i++)
	{
		if( m_TileCache->IsClean(i) )//if( m_TileCache->m_CacheStatus[i].dirty == false && m_TileCache->m_CacheStatus[i].reserved == true )// dirtyフラグがfalseでreservedがtrue
		{
			PageTables_Cleanup( m_TilesPageTableRefs[i].px, m_TilesPageTableRefs[i].py, m_TilesPageTableRefs[i].miplv, i, m_TilesPageTableRefs[i].mapid );
			
			m_TileCache->Free(i);
			//m_TileCache->m_CacheStatus[i].reserved	= false;
			//m_TileCache->m_CacheStatus[i].dirty		= false;
			//m_TileCache->m_numTiles--;

			m_TilesPageTableRefs[i].miplv	= -1;
			m_TilesPageTableRefs[i].mapid	= -1;
			m_TilesPageTableRefs[i].px		= -1;
			m_TilesPageTableRefs[i].py		= -1;

		}

	}// end of i loop

Check_Dirty:

	//================ 現在使用中のタイルには、"最近アクセスないよ"フラグを付ける ============//
	for( i=0; i<m_TileCache->GetTileCapacity(); ++i )
		m_TileCache->SetClean(i);
		//m_TileCache->m_CacheStatus[i].dirty = false;// ”最近使った”フラグをリセットする

	LeaveCriticalSection( &m_CriticalSection );
}


// タイルキャッシュをクリアする
void VirtualTexture::TileCache_Clear()
{
	int i;
	
	EnterCriticalSection( &m_CriticalSection );// TileCache,PageTable更新処理が割り込むのを防止するクリティカルセクション

	//============ タイルキャッシュ上で長期間参照されていないタイル領域を解放する ==========//
	for( i=0; i<m_TileCache->GetTileCapacity(); ++i )
	{
		if( m_TileCache->IsRererved(i) )//if( m_TileCache->m_CacheStatus[i].reserved == true )
		{
			PageTables_Cleanup( m_TilesPageTableRefs[i].px, m_TilesPageTableRefs[i].py, m_TilesPageTableRefs[i].miplv, i, m_TilesPageTableRefs[i].mapid );
			
//			m_TileCache->m_CacheStatus[i].reserved	= false;// タイルキャッシュの未使用領域を解放（領域を上書き可能にする）
//			m_TileCache->m_CacheStatus[i].dirty		= false;

			m_TilesPageTableRefs[i].miplv	= -1;
			m_TilesPageTableRefs[i].mapid	= -1;
			m_TilesPageTableRefs[i].px		= -1;
			m_TilesPageTableRefs[i].py		= -1;
		}
	}// end of i loop

	m_TileCache->FreeAll();
//	m_TileCache->m_numTiles=0;

	LeaveCriticalSection( &m_CriticalSection );
}






// miplv以下のVirtualTextureで，TileCache座標cx, cyを包含するPageTablesを全てEmptyにする
void VirtualTexture::PageTables_Cleanup(int px_curr, int py_curr, int miplv_curr, int tileid, int mapid)
{
	int miplv;
	int	px, py;
	
	page_table_st	*pCurr = &m_TableList[mapid].m_Tables[miplv_curr];

	if( tileid != pCurr->Page[px_curr][py_curr].tileid ) return;// タイルidが不一致になった時点で処理を終了する
	
	//tcout << _T("PageTables[") << id << _T("]->(") << curr_px << _T(",") << curr_py << _T(") is released.") << tendl;

	pCurr->Page[px_curr][py_curr].tileid = 0;
	pCurr->Page[px_curr][py_curr].curr_lv = 0;// レベルをゼロに戻す？
	pCurr->Page[px_curr][py_curr].status = PAGE_STATUS_EMPTY;	// リンク先を空にする
	pCurr->Page[px_curr][py_curr].locked = false;				// ページテーブルのロックを解除
	
	if(miplv_curr==m_TableList[mapid].m_numMIPs-1)	return;

	// PageTablesの子供４つに移動して，削除対称のPageTableをチェックする
	px = px_curr << 1;
	py = py_curr << 1;
	miplv = miplv_curr + 1;

	
	PageTables_Cleanup(px,   py,   miplv, tileid, mapid);
	PageTables_Cleanup(px+1, py,   miplv, tileid, mapid);
	PageTables_Cleanup(px+1, py+1, miplv, tileid, mapid);
	PageTables_Cleanup(px,   py+1, miplv, tileid, mapid);
}


// タイル描画処理（スレッドで処理する）
unsigned VirtualTexture::RunProcess(void *Param)
{
	EnterCriticalSection( &m_CriticalSection );

	ThreadParam param = *(ThreadParam *)Param;
	PageTables_Update(param.vtex_tc, param.miplv, param.mapid, param.du, param.dv, param.dn);
	
	LeaveCriticalSection( &m_CriticalSection );

	//Sleep(15);// タイルキャッシュ更新にCPU/GPUパワーが偏らないように実行速度にブレーキをかける

	return 0;
}


// スレッドプールにジョブを入れる
void VirtualTexture::SubmitJob(ThreadPool *pool, const Vec2f &vtex_tc, int miplv, int mapid,
							   const Vec3f &du, const Vec3f &dv, const Vec3f &dn)
{
	tParam.miplv	= miplv;
	tParam.vtex_tc	= vtex_tc;
	tParam.mapid	= mapid;
	tParam.du		= du;
	tParam.dv		= dv;
	tParam.dn		= dn;
	pool->SubmitJob(this, (void *)&tParam);		
}



// ページテーブルが低解像度タイルテクスチャを参照する
void VirtualTexture::PageTables_Upsample(int miplv_curr, int px_curr, int py_curr, int mapid)
{
	int i;
	int px_upper, py_upper;
	page_table_st *pCurr, *pUpper;

	pCurr = &m_TableList[mapid].m_Tables[miplv_curr];
	px_upper = px_curr;
	py_upper = py_curr;

	// ページテーブルを上に辿って，タイル参照ページが見つかったら中身をコピーして終了
	for(i=miplv_curr-1; i>=1; i--)// 各PageTableについて，，，
	{
		pUpper = &m_TableList[mapid].m_Tables[i];
		px_upper >>= 1;
		py_upper >>= 1;
		
		if(pUpper->Page[px_upper][py_upper].status == PAGE_STATUS_HIGHRES && pUpper->Page[px_upper][py_upper].locked==false)
		{
			pCurr->Page[px_curr][py_curr] = pUpper->Page[px_upper][py_upper];
			pCurr->Page[px_curr][py_curr].curr_lv = i;
			pCurr->Page[px_curr][py_curr].status = PAGE_STATUS_LOWRES;
			pCurr->Page[px_curr][py_curr].locked = false;
			break;
		}

	}// end of i loop

}











//############################################################################################################//
//######################################### 以下未使用コード #################################################//
//############################################################################################################//


// TileCaheの中身を仮想テクスチャ空間に射影して描画する
void VirtualTexture::DrawPageTables()
{
	/*
	int i;
	int px, py;
	int cx, cy;
	float	offset_x, offset_y, scale;

	// 頂点列
	float points[] =
	{
		0, 0, 0,  //v0
		0, 1, 0,  //v1
		1, 1, 0,  //v2
		1, 0, 0,  //v3
	};
	// テクスチャ座標
	float texcoord[] =
	{
		0, 0,  //v0
		0, 1,  //v1
		1, 1,  //v2
		1, 0,  //v3
	};

	// 頂点インデックス
	GLuint index[] =
	{
		0,1,2,3,
	};//インデックスの合計は4


	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, points);// 頂点の次元，数値型，stride?，頂点配列へのポインタ
	glTexCoordPointer(2, GL_FLOAT, 0, texcoord);

	glBindTexture(GL_TEXTURE_2D, m_TileCache);
	glEnable(GL_TEXTURE_2D);
	
	for(i=0; i<6; i++)
	{
		PageTable	*pCurr = &m_PageTables[2][i];
		scale = 1.0f;// / pow(2.0f, float(i)); /// (float)pCurr->tile_size;

		for(px=0; px<pCurr->tile_size; px++)
		{
			offset_x = float(px) - float(pCurr->tile_size)/2.0;

			for(py=0; py<pCurr->tile_size; py++)
			{
				if( pCurr->Page[px][py].status == PAGE_STATUS_EMPTY )	continue;// 空白ページテーブルは無視
	
				// タイルキャッシュを参照した四角を描画する
				offset_y = float(py) - float(pCurr->tile_size)/2.0;
				
				cx = pCurr->Page[px][py].tileid % m_CacheWidth;
				cy = pCurr->Page[px][py].tileid / m_CacheWidth;

				texcoord[0] = (float)cx / m_CacheWidth;
				texcoord[1] = (float)cy / m_CacheHeight;

				texcoord[2] = texcoord[0];
				texcoord[3] = texcoord[1]+1.0f / m_CacheHeight;

				texcoord[4] = texcoord[0]+1.0f / m_CacheWidth;
				texcoord[5] = texcoord[1]+1.0f / m_CacheHeight;

				texcoord[6] = texcoord[0]+1.0f / m_CacheWidth;
				texcoord[7] = texcoord[1];

				glPushMatrix();
				//glColor3f(color*0.5, color*0.5, color); // タイル解像度を色にする
				glTranslatef(offset_x+3800, offset_y+3800, 1.0f - (float)i / (float)m_numMIPs * 40+3800);
				glScalef(scale, scale, scale);
				glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, index);
				glPopMatrix();
			}// end of px loop
		}// ed of py loop
	
	}// end of i loop

	glDisable(GL_TEXTURE_2D);
	
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
*/
}


void VirtualTexture::DrawTileCache()
{/*
	glEnable(GL_TEXTURE_2D);

	glColor3f(1,1,1);
	glBindTexture(GL_TEXTURE_2D, m_TileCache);
	glBegin(GL_QUADS);
		glTexCoord2f(0,0);	glVertex2f(-1000, -500);
		glTexCoord2f(1,0);	glVertex2f(+1000, -500);
		glTexCoord2f(1,1);	glVertex2f(+1000, +500);
		glTexCoord2f(0,1);	glVertex2f(-1000, +500);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_TEXTURE_2D);
	*/
}