#define MV_NUMBER 16 
#define P_VALUE 15 

void Get_MotionVector(vector<short>& moVec, unsigned char ** r_frame, unsigned char** t_frame, int width, int height)
{
	moVec.clear();
	short mv_x = 0, mv_y = 0, x = 0, y = 0;
	unsigned char ** cp_block_r = new unsigned char *[MV_NUMBER];
	for (int i = 0; i < MV_NUMBER; i++)
		cp_block_r[i] = new unsigned char[MV_NUMBER];
	unsigned char ** cp_block_t = new unsigned char *[MV_NUMBER];
	for (int i = 0; i < MV_NUMBER; i++)
		cp_block_t[i] = new unsigned char[MV_NUMBER];


	for (int i = 0; i < height / MV_NUMBER; i++)
	{
		for (int j = 0; j < width / MV_NUMBER; j++)
		{
			x = j * MV_NUMBER;
			y = i * MV_NUMBER;
			int diff = INT_MAX;
			short temp_mv_x = 0;
			short temp_mv_y = 0;
			for (mv_y = -1 * P_VALUE; mv_y < P_VALUE; mv_y++)
			{
				for (mv_x = -1 * P_VALUE; mv_x < P_VALUE; mv_x++)
				{
					if (((y + mv_y) >= 0) && ((y + mv_y) <= (height - MV_NUMBER)) && ((x + mv_x) >= 0) && ((x + mv_x) <= (width - MV_NUMBER)))
					{
						int temp = 0;
						Div_ImgBlock_MV(r_frame, cp_block_r, y + mv_y, x + mv_x);
						Div_ImgBlock_MV(t_frame, cp_block_t, y, x);
						for (int k = 0; k < MV_NUMBER; k++)
						{
							for (int l = 0; l < MV_NUMBER; l++)
							{
								temp += abs((int)cp_block_r[k][l] - (int)cp_block_t[k][l]);
							}
						}
						if (temp < diff)
						{
							diff = temp;
							temp_mv_x = mv_x;
							temp_mv_y = mv_y;
						}
					}
				}
			}
			moVec.push_back(temp_mv_x);
			moVec.push_back(temp_mv_y);
		}
	}

	for (int i = 0; i < MV_NUMBER; i++)
		delete[] cp_block_t[i];
	delete[] cp_block_t;
	for (int i = 0; i < MV_NUMBER; i++)
		delete[] cp_block_r[i];
	delete[] cp_block_r;
}


//unsigned char 데이터를 MV_NUMBER*MV_NUMBER 블록에 (m,n)에서부터 MV_NUMBER, MV_NUMBER범위를 넣어주는 함수
void Div_ImgBlock_MV(unsigned char ** data, unsigned char ** block, int m, int n)
{
	for (int i = 0; i < MV_NUMBER; i++)
	{
		for (int j = 0; j < MV_NUMBER; j++)
		{
			block[i][j] = data[m + i][n + j];
		}
	}
}