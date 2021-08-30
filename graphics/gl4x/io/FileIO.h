#ifndef	FILE_IO_H
#define	FILE_IO_H


#include	<vector>
#include	<list>

#include	<oreore/common/Utility.h>
#include	<oreore/common/TString.h>


#ifndef __FILENAME__
#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#endif

namespace OreOreLib
{



	//##############################################################################################//
	//								ファイルパス群を取得するクラス									//
	//##############################################################################################//

	struct FilePath
	{
		tstring directory;
		tstring filename;
	};


	struct GL_SHADER_MACRO
	{
		tstring	Name;
		tstring	Definition;

		GL_SHADER_MACRO( const tstring &name, const tstring &def ){ Name = name; Definition = def; }
	};



	class FileGetter
	{
	public:

		FileGetter();
		~FileGetter();

		void AddExtension( const TCHAR *ext );
		void ClearExtension();

		void Scan( std::list<FilePath> &file_list, const tstring root_dir );// ルートディレクトリと拡張子を指定してファイル一覧を取得する


	private:

		std::vector<tstring>	m_Exts;	// 拡張子
										//bool						m_// ディレクトリ階層を探索するかどうか


		void ScanDirectory_rec( std::list<FilePath> &file_list, int cnt, const tstring dirname );// フォルダ階層を辿ってファイルパスを集める

	};




	//##############################################################################################//
	//										ヘルパー関数											//
	//##############################################################################################//


	// GL_SHADER_MACROを文字列に展開する
	static tstring ExtractMacro( const GL_SHADER_MACRO* pDefines )
	{
		tstring str;

		int i=0;

		while( !pDefines[i].Name.empty() )
		{
			str += _T( "#define " ) + pDefines[i].Name;
			if( !pDefines[i].Definition.empty() )
				str += _T( "	" ) + pDefines[i].Definition;

			str += _T( "\n" );

			i++;
		}

		return str;
	}


	// BOMを飛ばす
	static bool SkipBOM( tifstream& fin )
	{
		TCHAR test[4] = { 0 };
		fin.read( test, 3 );
		if( _tcscmp( test, _T("\xEF\xBB\xBF") )==0 )
			return true;
		fin.seekg( 0 );
		return false;
	}



	// テキストファイルをtstringに格納する
	static tstring textFileRead( const TCHAR* FilePass )
	{
		//setlocale( LC_ALL, "japanese" );
		TCHAR		*buf;
		tifstream	fin;

		fin.open( FilePass );

		if( !fin.is_open() )
		{
			tcout << "Could not open file: " << FilePass << "..." << tendl;
			return tstring( _T( "" ) );
		}

		fin.seekg( 0, std::ios::end ).tellg();// ファイル末尾を探す
		uint32 eofPos = uint32( fin.tellg() );// ファイル末尾インデックスを取得する

		fin.clear();

		fin.seekg( 0, tifstream::beg );	// ファイル先頭に戻る
		//bool hasBom = SkipBOM( fin );// BOMがある場合は無視する. // TODO: これやってもUNICODEで正しくシェーダー読み込めない。日本語が原因かも. 2017.11.18

		uint32 begPos = uint32( fin.tellg() );
		uint32 filesize = eofPos - begPos;
		//tcout << "file size = " << filesize << " [bytes]" << endl;

		buf	= new TCHAR[filesize+1];
		memset( buf, 0, filesize );// 0クリアしておく

		fin.read( buf, filesize );

		fin.close();
		buf[filesize] = '\0';
		tstring str( buf );
		//	tcout << str;

		delete[] buf;

		return str;
	}



	// strをdelimで分割して文字列を返す
	static std::vector<tstring> split( const tstring &str, const tstring &delim )
	{
		std::vector<tstring> res;
		size_t	current = 0,
			found,
			delimlen = delim.size();
		while( ( found = str.find( delim, current ) ) != tstring::npos )
		{
			tstring aaa = tstring( str, current, found - current );

			if( aaa.size() > 0 )
				res.push_back( aaa/*tstring(str, current, found - current)*/ );
			current = found + delimlen;
		}
		tstring aaa = tstring( str, current, str.size() - current );
		if( aaa.size() > 0 )
			res.push_back( aaa/*tstring(str, current, str.size() - current)*/ );
		return res;
	}




}// end of namespace



#endif	// FILE_IO_H //