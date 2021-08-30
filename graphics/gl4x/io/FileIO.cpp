#include	"FileIO.h"


#include	<Windows.h>
#include	<iostream>
#include	<fstream>
using namespace std;



namespace OreOreLib
{


FileGetter::FileGetter()
{


}



FileGetter::~FileGetter()
{


}



void FileGetter::AddExtension( const TCHAR *ext )
{
	tstring extension = tstring(_T(".")) + ext;

	tcout << extension.c_str() << endl;

	m_Exts.push_back( extension );
	
}


void FileGetter::ClearExtension()
{
	m_Exts.clear();
}



// ルートディレクトリと拡張子を指定してファイル一覧を取得する
void FileGetter::Scan( std::list<FilePath> &file_list, const tstring root_dir )
{
	//================ ディレクトリの設定 =================//
	// カレントディレクトリ名を覚えておく
	TCHAR origDir[_MAX_PATH];
	GetCurrentDirectory( _MAX_PATH, origDir );

	// ファイル取得するディレクトリに移動する
	SetCurrentDirectory( root_dir.c_str() );


	//=============== ファイル一覧を作成する =============//
	ScanDirectory_rec( file_list, 0, root_dir );


	//=========== カレントディレクトリを元に戻す =========//
	SetCurrentDirectory( origDir );

}





void FileGetter::ScanDirectory_rec( std::list<FilePath> &file_list, int cnt, const tstring dirname )
{
    WIN32_FIND_DATA fd;
    HANDLE h;
	
    // ハンドルを取得する
    h = FindFirstFileEx( tstring( dirname + _T("*") ).c_str(), FindExInfoStandard, &fd, FindExSearchNameMatch, NULL, 0 );
	
    if( INVALID_HANDLE_VALUE == h )
	{
        tcout << "Error occured at directory " << dirname.c_str() << endl;
        return;
    }

    do
	{
        if( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
		{
            // ディレクトリの場合
            if ( _tcscmp( fd.cFileName, _T(".") ) && _tcscmp( fd.cFileName, _T("..") ) ) 
			{
				// .と..は処理しない
				// print_cnt( cnt );
				//printf( "ディレクトリ - %s\n", fd.cFileName );
				if( !(fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) )// 隠しディレクトリは無視
					ScanDirectory_rec( file_list, cnt + 1, dirname + fd.cFileName + _T("\\") );
            }
        }
        else
		{
			// ファイルの場合
			if( !(fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) )
			{
				FilePath	path = { dirname, tstring(fd.cFileName) };

				for( int i=0; i<m_Exts.size(); ++i )
				{
					if( path.filename.find(m_Exts[i]) != tstring::npos )	file_list.push_back( path );
					//if( path.filename.find(".jpg") != tstring::npos )	file_list.push_back( path );
				}// end of i loop
			}
        }
    // 次のファイルを検索する
    } while ( FindNextFile( h, &fd ) );

    // ハンドルを閉じる
    FindClose( h );
}


}// end of namespace