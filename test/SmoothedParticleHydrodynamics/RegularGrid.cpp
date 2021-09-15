#include "RegularGrid.h"



//#########################################################################################################//
//########################################### Public Functions  ###########################################//
//#########################################################################################################//

//	[関数名]	RegularGrid_Init
//	[引数]		bb：BoundingBox対角頂点の３次元座標値，bin：セルの幅
//	[戻り値]
//　[内容]		bbの近時ボリューム（幅bin）を作成する．
void RegularGrid_Init(RegularGrid *rg, Vec3d bb[2], double bin)
{
	int i;
	double	diff;
	double	len;
	double bin_inv;
	
	rg->Bin = bin;// セルの幅
	bin_inv = 1.0 / bin;

	//================== グリッド各軸方向の分割数を決める ==================//
	for(i=0; i<3; i++)
	{
		len = (bb[1].xyz[i] - bb[0].xyz[i])*1.1;// 余裕を見て軸長さの1.1倍取る
		rg->Size[i] = (int)ceil(len * bin_inv);// Binでの軸分割数
		
		diff = rg->Bin * (double)rg->Size[i] - len;// BoundingBox幅と入力bb幅との長さ差分
		
		rg->Boundary_min.xyz[i] = bb[0].xyz[i] - diff*0.5;// 差分をBondingBoxの端点に半分ずつ割り振る
		rg->Boundary_max.xyz[i] = bb[1].xyz[i] + diff*0.5;// 差分をBondingBoxの端点に半分ずつ割り振る
	}
	
	rg->cellNum = rg->Size[0] * rg->Size[1] * rg->Size[2];

	//======================== 粒子ID格納用セルの確保 =======================//
	rg->Cells = (int **) malloc (sizeof(int*) * rg->cellNum);
	rg->Capacity = (int *) malloc (sizeof(int) * rg->cellNum);
	rg->Elms = (int *) malloc (sizeof(int) * rg->cellNum);
	
	if (rg->Elms == NULL)
		cout << "Cannot Allocate Memory\n";
	
	for(i=0; i<rg->cellNum; i++)
	{
		rg->Capacity[i] = 1;
		rg->Elms[i] = 0;
		rg->Cells[i] = (int *) malloc (sizeof(int));
	}

}



//	[関数名]	RegularGrid_InsertID
//	[引数]		id：番号, pos：３次元座標値
//	[戻り値]	登録の有無：true→登録した，false→登録しなかった
//　[内容]		座標値posを包含するセルに値idを追加する
int RegularGrid_InsertID(RegularGrid *rg, int id, const Vec3d& pos)
{
	int i;
	int newsize;
	int index;
	int ix, iy, iz;
	double bin_inv;

	//=========== 粒子の位置が領域外の場合は処理を中止する =============//
	for(i=0; i<3; i++)
		if(pos.xyz[i] < rg->Boundary_min.xyz[i] || pos.xyz[i] > rg->Boundary_max.xyz[i]) return 0;
	
	//=========== 粒子の座標値を3次元セルインデックスに変換する =========//
	bin_inv = 1.0 / rg->Bin;
	
	ix = (int)/*ceil*/( (pos.x - rg->Boundary_min.x) * bin_inv );
	iy = (int)/*ceil*/( (pos.y - rg->Boundary_min.y) * bin_inv );
	iz = (int)/*ceil*/( (pos.z - rg->Boundary_min.z) * bin_inv );
	
	index = rg->Size[1]*rg->Size[2]*ix + rg->Size[2]*iy + iz;
	
	//==================== セルに粒子IDを登録する =======================//
	if (rg->Elms[index] == rg->Capacity[index])
	{
		newsize = (rg->Elms[index]+1) * 2;
		rg->Cells[index] = (int *) realloc (rg->Cells[index], sizeof(int) * newsize);
		rg->Capacity[index] = newsize;
	}

	rg->Cells[index][rg->Elms[index]] = id;
	rg->Elms[index]++;
	
	return 1;
}



//	[関数名]	RegularGrid_InsertID
//	[引数]		id：番号, pos：３次元座標値
//	[戻り値]	-
//　[内容]		cellindexで指定したセルに粒子ID(id)を登録する
void RegularGrid_InsertID(RegularGrid *rg, int id, int cellindex)
{
	int newsize;
	
	//======== 粒子ID格納領域の容量チェックと追加確保 =========//
	if (rg->Elms[cellindex] == rg->Capacity[cellindex])
	{
		newsize = (rg->Elms[cellindex]+1) * 2;
		rg->Cells[cellindex] = (int *) realloc (rg->Cells[cellindex], sizeof(int) * newsize);
		rg->Capacity[cellindex] = newsize;
	}

	//=================== idの登録 ========================//
	rg->Cells[cellindex][rg->Elms[cellindex]] = id;
	rg->Elms[cellindex]++;
}



//	[関数名]	RegularGrid_Clear
//	[引数]		なし
//	[戻り値]	なし
//　[内容]		各セルに登録した粒子IDをクリアする（実際は登録粒子数を0にセットするだけ）
void RegularGrid_Clear(RegularGrid *rg)
{
	int i;
	
	for(i=0; i<rg->cellNum; i++)
		rg->Elms[i] = 0;

}



//	[関数名]	RegularGrid_Info
//	[引数]		なし
//	[戻り値]	なし
//　[内容]		各セルの要素数を0にセットする
void RegularGrid_Info(const RegularGrid *rg)
{
	cout << "################# RegularGridクラス情報 ##############" << endl;
	cout << "Grid size = "<<rg->Size[0] << " * " << rg->Size[1] << " * " << rg->Size[2] << endl;
	cout << "Allocated memory...." << endl;
	cout << "	Cells: " << (sizeof(int) * *(int*)(rg->Capacity)) / 1024 << " KB" << endl;

}




//#########################################################################################################//
//########################################## Private Functions  ###########################################//
//#########################################################################################################//








/*
//	[関数名]	GetNeighbors
//	[引数]		id：番号, pos：３次元座標値
//	[戻り値]	idsに登録した近傍IDの数
//　[内容]		pos近傍セル(27個)に含まれるidを取得する
int RegularGrid::GetNeighbors(std::vector<int> &ids, double pos[3])
{
	int x,y,z,k;
	int elms;
	int curr,index; 
	int ix,iy,iz;
	int ix_min, iy_min, iz_min;// セルインデックス下限値
	int ix_max, iy_max, iz_max;// セルインデックス上限値
	
	if(ids.size()==0) ids.resize(1);
	
	//+++++ BoundingBoxセル幅と位置から，該当セルを見つける +++++//
	ix = (int)ceil( (pos[0] - BoundingBox[0][0]) / Bin );
	iy = (int)ceil( (pos[1] - BoundingBox[0][1]) / Bin );
	iz = (int)ceil( (pos[2] - BoundingBox[0][2]) / Bin );
	
	ix_min = Max(0, ix-1);
	iy_min = Max(0, iy-1);
	iz_min = Max(0, iz-1);

	ix_max = Min(Size[0]-1, ix+1);
	iy_max = Min(Size[1]-1, iy+1);
	iz_max = Min(Size[2]-1, iz+1);

	//++++++++++++++++++ 各セルのIDをidsに書き込む +++++++++++++++++++//
	elms = 0;// 該当セル群（27セル）に格納されたIDの個数
	curr = 0;// ids配列上の値を格納する場所
	for(x=ix_min; x<=ix_max; x++){
		for(y=iy_min; y<=iy_max; y++){
			for(z=iz_min; z<=iz_max; z++)
			{
				index = IX(x,y,z);
				elms += Cells[index].size();// idsの要素数を求める.
				
				if(elms > ids.size())// 要素数に満たない場合は２倍のメモリを確保
					ids.resize( elms*2 );
				
				for(k=0; k<Cells[index].size(); k++)
				{
					ids[curr] = Cells[index][k];
					curr++;
				}
			}// end of z loop
		}// end of y loop
	}// end of x loop


	return elms;
}
*/



void RegularGrid_RenderGL(const RegularGrid *rg, int x, int y, int z)
{
	const double& bin = rg->Bin;
	const Vec3d& min = rg->Boundary_min;
	const Vec3d& max = rg->Boundary_max;


	glPushMatrix();
	glBegin(GL_LINE_STRIP);

		glVertex3d(min.x, min.y, min.z);
		glVertex3d(min.x, min.y, max.z);
		glVertex3d(min.x, max.y, max.z);
		glVertex3d(min.x, max.y, min.z);

		glVertex3d(max.x, min.y, min.z);
		glVertex3d(min.x, min.y, min.z);
		glVertex3d(min.x, max.y, min.z);
		glVertex3d(max.x, max.y, min.z);

		glVertex3d(max.x, min.y, max.z);
		glVertex3d(max.x, min.y, min.z);
		glVertex3d(max.x, max.y, min.z);
		glVertex3d(max.x, max.y, max.z);

		glVertex3d(min.x, min.y, max.z);
		glVertex3d(max.x, min.y, max.z);
		glVertex3d(max.x, max.y, max.z);
		glVertex3d(min.x, max.y, max.z);

		glVertex3d(max.x, max.y, max.z);
		glVertex3d(max.x, max.y, min.z);
		glVertex3d(min.x, max.y, min.z);
		glVertex3d(min.x, max.y, max.z);

		glVertex3d(min.x, min.y, min.z);
		glVertex3d(max.x, min.y, min.z);
		glVertex3d(max.x, min.y, max.z);
		glVertex3d(min.x, min.y, max.z);

	glEnd();
	glPopMatrix();

}