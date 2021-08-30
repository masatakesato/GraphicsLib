#ifndef	CACHE_DATA_MANAGER_H
#define	CACHE_DATA_MANAGER_H

#include	<windows.h>
#include	<process.h>

#include	<oreore/MathLib.h>
#include	"./container/RingQueue.h"




namespace OreOreLib
{


	//##################################################################################//
	//						キャッシュデータの状態を管理するクラス						//
	//##################################################################################//

	class CacheDataManager
	{
	public:

		CacheDataManager();// デフォルトコンストラクタ
		CacheDataManager( int max_slots );// コンストラクタ
		~CacheDataManager();// デストラクタ

		void Init( int max_slots );
		void Release();

		int numMaxSlots() const				{ return m_numMaxSlots; }			// キャッシュが保持するスロット数を返す
		int numReservedSlots() const		{ return m_numReservedSlots; }		// 使用中のスロット数を返す

		bool IsFull() const					{ return m_FreeSlots.IsEmpty(); }	// スロットが満杯かどうかチェックする(m_FreeSlotsが空なら満杯)
		bool IsEmpty() const				{ return m_FreeSlots.IsFull(); }	// 全スロットが未使用かどうかチェックする(m_FreeSlotsが満杯ならtrue)
		bool IsRererved( int slot_id) const	{ return (m_CacheStatus + slot_id)->reserved; }
		bool IsDirty( int slot_id ) const	{ return (m_CacheStatus + slot_id)->reserved && (m_CacheStatus + slot_id)->dirty; }	// 最近データへのアクセスがあったかどうか調べる
		bool IsClean( int slot_id ) const	{ return (m_CacheStatus + slot_id)->reserved && !(m_CacheStatus + slot_id)->dirty; }// アクセスなしの放置状態になっているかどうか調べる
		
		int ReserveSlot( int numslots=1 );					// 未使用スロットを見つけて予約する．
		bool SetClean( int slot_id );		// 指定スロットのアクセスフラグをCleanに設定する
		bool SetDirty( int slot_id );		// 指定スロットのアクセスフラグをDirtyに設定する
		bool FreeSlot( int slot_id );		// 連続確保したスロット群を解放し、未使用状態にする

		void Clear();						// キャッシュを初期状態（何もデータを保持しない）にする

		bool GetSerialSlots( int start_slot, int num_slots, int *slots );	// 特殊関数. 連続確保したスロット群を配列に詰める. 複数ある場合はtrue, 単一の場合はfalse
		int* GetLinkBuffer() const	{ return m_LinkBuffer; }


	private:

		//================== キャッシュ状態 =================//
		struct CacheStatus
		{
			bool dirty;		// テクスチャ領域へのアクセスフラグ
			bool reserved;	// テクスチャ領域の予約フラグ
			bool front;		// 最初の要素
		};

		int				m_numMaxSlots;		// データスロット最大数
		int				m_numReservedSlots;	// 予約可能なデータスロットの数
		CacheStatus		*m_CacheStatus;		// タイルキャッシュの使用状況
		RingQueue<int>	m_FreeSlots;		// 空きスロットの番号リスト


		int				*m_LinkBuffer;		// 次の要素へのリンク


		CacheDataManager( const CacheDataManager& obj ){}// コピーコンストラクタ

		int ReserveSingleSlot();					// 未使用スロットを見つけて予約する．
		bool FreeSingleSlot( int slot_id );		// 指定スロットを未使用状態にする

	};



	// 指定スロットのデータを「しばらくアクセスがない=汚れていない」状態にする
	inline bool CacheDataManager::SetClean( int slot_id )
	{
		if( slot_id >= m_numMaxSlots )	return false;

		int current_slot		= slot_id;
		CacheStatus *pstatus	= NULL;

		while( current_slot != -1 )
		{
			pstatus = m_CacheStatus + current_slot;

			if( !pstatus->reserved )	return false;	
			pstatus->dirty	= false;

			current_slot = *(m_LinkBuffer + current_slot);
		}

		
		return true;
	}


	//inline bool CacheDataManager::SetClean( int slot_id )
	//{
	//	if( slot_id >= m_numMaxSlots )	return false;

	//	(m_CacheStatus + slot_id)->dirty	= false;
	//	return true;
	//}





	// 指定スロットのデータを「最近アクセスした=触って垢がついた」状態にする
	inline bool CacheDataManager::SetDirty( int slot_id )
	{
		if( slot_id >= m_numMaxSlots )	return false;

		int current_slot		= slot_id;
		CacheStatus *pstatus	= NULL;

		while( current_slot != -1 )
		{
			pstatus = m_CacheStatus + current_slot;

			if( !pstatus->reserved )	return false;
			pstatus->dirty	= true;

			current_slot = *(m_LinkBuffer + current_slot);
		}

		return true;
	}


	//inline bool CacheDataManager::SetDirty( int slot_id )
	//{
	//	if( slot_id >= m_numMaxSlots )	return false;

	//	CacheStatus *pstatus = m_CacheStatus + slot_id;

	//	if( !pstatus->reserved )	return false;
	//	pstatus->dirty	= true;

	//	return true;
	//}




	// キャッシュの未使用領域を指定数探して予約する。戻り値は先頭スロット(予約失敗した場合は-1を返す)
	inline int CacheDataManager::ReserveSlot( int numslots )
	{	
		if( m_FreeSlots.IsEmpty() )	return -1;	// フリースロットが空の場合は処理中止
		if( numslots > m_numMaxSlots - m_numReservedSlots ) return -1;

		//================== 先頭スロットを予約する =================//
		int start_slot	= ReserveSingleSlot();
		m_CacheStatus[ start_slot ].front = true;

		//=============== 残りのスロットを予約する ==================//
		int prev_slot	= start_slot;

		for( int i=0; i<numslots-1; ++i )
		{
			// スロットを新たに予約する
			int newslot	= ReserveSingleSlot();

			// 直前スロットのリンク先を、新しく予約したnewslotに設定する
			m_LinkBuffer[ prev_slot ]	= newslot;

			prev_slot	= newslot;
		}

		return start_slot;
	}



	// 連続確保したスロット群を解放し、未使用状態にする
	inline bool CacheDataManager::FreeSlot( int slot_id )
	{
		if( slot_id >= m_numMaxSlots )
			return false;

		if( m_CacheStatus[ slot_id ].front==false )
			return false;
		

		m_CacheStatus[ slot_id ].front = false;

		int current_slot	= slot_id;
		int next_slot;

		while( current_slot != -1 )
		{
			next_slot		= m_LinkBuffer[ current_slot ];

			// リンク先スロットを解放する
			FreeSingleSlot( current_slot );

			// 現在のスロットが保持するリンク先情報を、バッファから削除する
			m_LinkBuffer[ current_slot ] = -1;

			// カレントスロットを次に進める
			current_slot	= next_slot;
		}
		
		return true;
	}




	// 全スロットを未使用状態にする
	inline void CacheDataManager::Clear()
	{
		m_FreeSlots.Clear();

		for( int i=0; i<m_numMaxSlots; ++i )
		{
			m_CacheStatus[i].reserved	= false;
			m_CacheStatus[i].dirty		= false;
			m_CacheStatus[i].front		= false;

			m_FreeSlots.Enqueue(i);

			m_LinkBuffer[i]	= -1;


		}

		m_numReservedSlots	= 0;
	}



	inline bool CacheDataManager::GetSerialSlots( int start_slot, int num_slots, int *slots )
	{
		int counter = 0;
		int current_slot	= start_slot;

		slots[ counter++ ] = start_slot;

		while( current_slot != -1 && counter < num_slots )
		{
			slots[ counter++ ]	= m_LinkBuffer[ current_slot ];
			current_slot	= m_LinkBuffer[ current_slot ];
		}

		return true;
	}



	// キャッシュの未使用領域を探して予約する
	inline int CacheDataManager::ReserveSingleSlot()
	{	
		if( m_FreeSlots.IsEmpty() )	return -1;	// フリースロットが空の場合は処理中止

		//=========== 空きスロットのIDを取得する ==============//
		int freeSlot	= m_FreeSlots.Dequeue();

		//=========== 空きスロットを使用状態にする ===========//
		m_CacheStatus[ freeSlot ].reserved	= true;
		m_CacheStatus[ freeSlot ].dirty		= true;

		//======= 使用中スロット数をインクリメントする =======//
		m_numReservedSlots++;

		return freeSlot;
	}



	// 指定スロットを解放し、未使用状態にする
	inline bool CacheDataManager::FreeSingleSlot( int tile_id )
	{
		if( tile_id >= m_numMaxSlots )
			return false;

		//========== 指定スロットを空き状態にする ============//
		m_CacheStatus[ tile_id ].reserved	= false; // タイル領域を予約する
		m_CacheStatus[ tile_id ].dirty		= false;// アップロード直後はアクセス痕跡を残す

		//========== 空きスロットリストに登録する ============//
		m_FreeSlots.Enqueue( tile_id );

		//========= 使用中スロット数をデクリメントする =======//
		m_numReservedSlots--;


		return true;
	}


}// end of namespace



#endif	// CACHE_DATA_MANAGER_H //