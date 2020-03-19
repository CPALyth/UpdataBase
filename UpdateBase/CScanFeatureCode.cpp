#include "CScanFeatureCode.h"

/********************************************************************************************/
/* Description	: �ֽڼ� ת ʮ�������ַ���
/* Return		: �޷���ֵ
/* Time			: 2020/01/24 11:46
/* Remark		: szpHexBufΪOUT����
/********************************************************************************************/
void BytesToHexStr(BYTE* nbData, DWORD nbDataSize, char* szpHexBuf) {
	for (DWORD i = 0; i < nbDataSize; i++)
	{
		ByteToHex(&szpHexBuf[2*i],nbData[i]);
	}
}

/********************************************************************************************/
/* Description	: ��һ���ֽڱ��ʮ�������ַ���
/* Return		: �޷���ֵ
/* Time			: 2020/01/26 16:13
/* Remark		: 
/********************************************************************************************/
void ByteToHex(char * szpBuffHex, BYTE c)
{
	int high = c / 16;		//ȡ�ֽڸ�λ:0-15
	int low = c % 16;		//ȡ�ֽڵ�λ:0-15
	//��λ����
	if (high > 9)	szpBuffHex[0] = 'A' + high - 10;
	else    szpBuffHex[0] = '0' + high;
	//��λ����
	if (low > 9)	szpBuffHex[1] = 'A' + low - 10;
	else    szpBuffHex[1] = '0' + low;
}

/********************************************************************************************/
/* Description	: ��һ���ַ���ɴ�д
/* Return		: һ���ַ������Сд��ĸ���ͷ��ش�д��ĸ��������ֱ�ӷ���������
/* Time			: 2020/01/24 16:46
/* Remark		: 
/********************************************************************************************/
char CharToUp(char c)
{
	if (c >= 'a' && c <= 'z')
	{
		return c - 0x20;
	}
	return c;
}

/********************************************************************************************/
/* Description	: ���ַ����е��ַ�ȫ��ת��Ϊ��д��ĸ && ��ʮ�����Ƶ�ͨ����滻Ϊ��X��
/* Return		: �޷���ֵ
/* Time			: 2020/01/24 17:03
/* Remark		: 
/********************************************************************************************/
void StrToUp(char* szpHexStr)
{
	DWORD ndStrLen = strlen(szpHexStr);
	for (DWORD i = 0; i <ndStrLen; i++)
	{
		szpHexStr[i] = CharToUp(szpHexStr[i]);	//��ת��Ϊ��д
		if (IsHexChar(szpHexStr[i])==FALSE)
		{
			szpHexStr[i] = 'X';	//������ַ�����ʮ�����ƣ����滻Ϊ��X��
		}
	}
}

/********************************************************************************************/
/* Description	: �Ƚ�ʮ��λ�ַ����Ƿ���ͬ
/* Return		: ���ַ�����ͬ �����棬���򷵻ؼ�
/* Time			: 2020/01/24 16:03
/* Remark		: 
/********************************************************************************************/
BOOL HexStrCmp(const char * HexStr1, const char * HexStr2)
{
	DWORD ndHexLen1 = strlen(HexStr1);
	DWORD ndHexLen2 = strlen(HexStr2);
	if (ndHexLen1 > ndHexLen2)	ndHexLen1 = ndHexLen2;
	for (DWORD i = 0; i < ndHexLen1; i++)
	{
		if (HexStr1[i]=='X' || HexStr2[i]=='X')	continue;	//��ͨ���������
		if (HexStr1[i] == HexStr2[i])	continue;
		return FALSE;
	}
	return TRUE;
}

/********************************************************************************************/
/* Description	: ɨ��ָ�����̵�������
/* Return		: �ɹ��ҵ��������������ʼVA��ʧ�ܷ���NULL
/* Time			: 2020/01/25 12:03
/* Remark		: 
/********************************************************************************************/
#define PAGESIZE 4096	//��ҳ��ȡ��ÿ�ζ�ȡ4096���ֽ�
DWORD ScanFeatureCode(HANDLE hProcess, char* szpFeatureCode,
	DWORD ndStartAddress, DWORD ndEndAddress)
{
	//�Ȱ��ִ��е��ַ�ȫ��תΪ��д�����滻ͨ���Ϊ��X��
	StrToUp(szpFeatureCode);
	//��ȡ�������ִ���next����
	int* next = new int[strlen(szpFeatureCode)];
	GetNext(szpFeatureCode, next);
	//��ȡʮ��������������ֽ���
	DWORD ndLenHexCode = strlen(szpFeatureCode) / 2;
	//����һ������ڴ��ֽ����ݵĻ���������СΪ ҳ�泤��+�ֽ���+NULL
	BYTE* nbDataBuf = new BYTE[PAGESIZE + ndLenHexCode + 2];
	//ʵ�ʶ�ȡ�Ļ�������С
	DWORD ndByReadSize = 0;
	//��ҳ����������
	for (DWORD ndCurAddr = ndStartAddress; ndCurAddr < ndEndAddress - ndLenHexCode; ndCurAddr += PAGESIZE)
	{
		//д�� ��Ϸ���̵��ڴ�ҳ�ֽ� �� nbDataBuf
		ReadProcessMemory(hProcess, (LPCVOID)ndCurAddr, nbDataBuf, PAGESIZE + ndLenHexCode + 2, &ndByReadSize);

		//д�� nbDataBuf �� szpTempHex,�������ֽ������ܳ���256�ֽ�
		char szpTempHex[(PAGESIZE + 256 + 2) * 2] = "";
		BytesToHexStr(nbDataBuf, ndByReadSize, szpTempHex);	// PAGESIZE + ndLenHexCode + 2

		//��char*���͵�C����ַ���ת��Ϊstring���͵ı���
		string FeatureStr = szpFeatureCode;
		string MemberStr = szpTempHex;

		//��ȡ�ִ�szpTempHex��FeatureStrƥ��ɹ�ʱ����ʼ�±�
		int iIndex = GetIndexByKMP(MemberStr, FeatureStr, next);
		if (iIndex != -1)
		{
			return ndCurAddr + iIndex / 2;
		}

	}
	return NULL;
}

/********************************************************************************************/
/* Description	: ����Ӵ�T��next����
/* Return		: ����next�����ָ��
/* Time			: 2020/01/26 17:54
/* Remark		: 
/********************************************************************************************/
int* GetNext(string T,int next[]) {
	UINT len = T.length();
	next[0] = 0;
	//��������ƥ�䴮
	for (UINT i=1,j=0; i< len; i++)
	{
		//���ж��Ƿ���ͨ������еĻ��ͱȺ�����ַ���������һ��ѭ��
		if (T.at(i) == 'X' || T.at(j) == 'X')
		{
			j++;	next[i] = j;
			continue;
		}
		while (j>0 && T.at(i)!=T.at(j))
		{
			j = next[j - 1];	
		}
		if (T.at(i) == T.at(j))
		{
			j++;
		}
		next[i] = j;
	}
	//����next����,��������ƥ�䴮��������,����next[0]=-1
	for (UINT j = len-1; j > 0; j--)
	{
		next[j] = next[j - 1];
	}
	next[0] = -1;
	return next;
}

/********************************************************************************************/
/* Description	: ����KMP�㷨,��ȡ�´ο�ʼƥ����Ӵ��±�
/* Return		: �ɹ������´ο�ʼƥ����Ӵ��±꣬ʧ�ܷ���-1
/* Time			: 2020/01/26 23:22
/* Remark		: SΪĸ����TΪ�Ӵ���
/********************************************************************************************/
int GetIndexByKMP(string S, string T, int next[]) {
	int len1 = S.length();
	int len2 = T.length();
	int i=0,j=0;
	while ( i<len1 && j<len2 )
	{
		if (j==-1 || S.at(i)==T.at(j) || T.at(j)=='X')	//ͨ�������
		{
			i++;	j++;
		}
		else
			j = next[j];
	}
	//ƥ�����
	if (j >= len2)	//ƥ��ɹ�
		return i - len2;
	else
		return -1;
}

/********************************************************************************************/
/* Description	: �ж�һ���ַ��Ƿ���ʮ�������ַ�
/* Return		: ���򷵻��棬���򷵻ؼ�
/* Time			: 2020/01/27 17:13
/* Remark		: 
/********************************************************************************************/
BOOL IsHexChar(char c)
{
	if (c>='0' && c<='9')
	{
		return TRUE;
	}
	if (c>='A' && c<='F')
	{
		return TRUE;
	}
	return FALSE;
}

/********************************************************************************************/
/* Description	: �����������ַ��������ַ
/* Return		: �򷵻��棬���򷵻ؼ�
/* Time			: 2020/01/28 13:28
/* Remark		: 
/********************************************************************************************/
DWORD ReadBaseAddr(HANDLE hProcess, DWORD ndAddr)
{
	DWORD ndBaseAddr = NULL;
	BOOL bRet = ReadProcessMemory(hProcess, (LPCVOID)ndAddr, &ndBaseAddr, 4,NULL);
	return ndBaseAddr;
}

/********************************************************************************************/
/* Description	: һ�����»�ַ
/* Return		: 
/* Time			: 2020/02/09 00:14
/* Remark		: 
/********************************************************************************************/
void OneKeyUpdateBase(HANDLE hProcess)
{
	DWORD ndLocateAddr = NULL;
	DWORD ndBaseAddr = NULL;
	char szpFeatureCode[256] = "";

	//Base_GameWndHandle
	strcpy_s(szpFeatureCode, "8B086A1652508B4134");
	ndLocateAddr = ScanFeatureCode(hProcess, szpFeatureCode);	//��λ������
	ndBaseAddr = ReadBaseAddr(hProcess, ndLocateAddr - 4);		//��ȡ��ַ
	printf("#define Base_GameWndHandle 0x%08X	//��Ϸ�����ھ��\n\n", ndBaseAddr);

	//Base_AllObjList
	strcpy_s(szpFeatureCode, "83C404A308C0C0008B018B50045757");
	ndLocateAddr = ScanFeatureCode(hProcess, szpFeatureCode);	//��λ������
	ndBaseAddr = ReadBaseAddr(hProcess, ndLocateAddr - 4);		//��ȡ��ַ
	printf("#define Base_AllObjList 0x%08X		//���ж����б��ַ\n", ndBaseAddr);
	printf("										//�ض������ַ��[���ж����б��ַ+4*ID]\n\n");

	//Base_RoleProperty
	strcpy_s(szpFeatureCode, "558BEC83EC085356B8");
	ndLocateAddr = ScanFeatureCode(hProcess, szpFeatureCode);	//��λ������
	ndBaseAddr = ReadBaseAddr(hProcess, ndLocateAddr + 9);		//��ȡ��ַ
	printf("#define Base_RoleProperty 0x%08X		//�������Ի�ַ\n\n", ndBaseAddr);

	//Base_EquipList
	strcpy_s(szpFeatureCode, "BF3C0400008D9B00000000833C070074**8B0C07");
	ndLocateAddr = ScanFeatureCode(hProcess, szpFeatureCode);	//��λ������
	ndBaseAddr = ReadBaseAddr(hProcess, ndLocateAddr - 4);		//��ȡ��ַ
	printf("#define Base_EquipList 0x%08X			//װ���б��ַ\n", ndBaseAddr);
	//Base_ShopList
	strcpy_s(szpFeatureCode, "6A0050E8********8B4F0883C40C51B9");
	ndLocateAddr = ScanFeatureCode(hProcess, szpFeatureCode);	//��λ������
	ndBaseAddr = ReadBaseAddr(hProcess, ndLocateAddr + 16);		//��ȡ��ַ
	printf("#define Base_ShopList 0x%08X			//�̵��б��ַ\n", ndBaseAddr);
	//Base_DepotList
	strcpy_s(szpFeatureCode, "C78134160000080000008B152C69E402899A6C020000");
	ndLocateAddr = ScanFeatureCode(hProcess, szpFeatureCode);	//��λ������
	ndBaseAddr = ReadBaseAddr(hProcess, ndLocateAddr - 4);		//��ȡ��ַ
	printf("#define Base_DepotList 0x%08X			//�ֿ��б��ַ\n", ndBaseAddr);
	//Base_BackPackList
	strcpy_s(szpFeatureCode, "8B848A3C0400008BB0E40C00008BB8E80C00008BC60BC7");
	ndLocateAddr = ScanFeatureCode(hProcess, szpFeatureCode);	//��λ������
	ndBaseAddr = ReadBaseAddr(hProcess, ndLocateAddr - 4);		//��ȡ��ַ
	printf("#define Base_BackPackList 0x%08X		//�����б��ַ\n", ndBaseAddr);
	//Call_UseObjForIndex
	strcpy_s(szpFeatureCode, "8B87601C00005651508BCFE8");
	ndLocateAddr = ScanFeatureCode(hProcess, szpFeatureCode);	//��λ������
	ndBaseAddr = ReadBaseAddr(hProcess, ndLocateAddr + 12);		//��ȡ��ַ
	printf("#define Call_UseObjForIndex 0x%08X	//������Ʒʹ��CALL\n", ndBaseAddr + ndLocateAddr + 16);
	//Call_SendData
	strcpy_s(szpFeatureCode, "6689B5FED7FFFF66899500D8FFFF66898502D8FFFFE8");
	ndLocateAddr = ScanFeatureCode(hProcess, szpFeatureCode);	//��λ������
	ndBaseAddr = ReadBaseAddr(hProcess, ndLocateAddr + 22);		//��ȡ��ַ
	printf("#define Call_SendData 0x%08X		//��������CALL\n", ndBaseAddr + ndLocateAddr + 26);
	//Call_SendData_Ecx
	ndBaseAddr = ReadBaseAddr(hProcess, ndLocateAddr - 4);		//��ȡ��ַ
	printf("#define Call_SendData_Ecx 0x%08X	//��������CALL��Ecx\n\n", ndBaseAddr);

	//Base_NearObjList
	strcpy_s(szpFeatureCode, "8B118B420453536A02FFD003F7");
	ndLocateAddr = ScanFeatureCode(hProcess, szpFeatureCode);	//��λ������
	ndBaseAddr = ReadBaseAddr(hProcess, ndLocateAddr -4);		//��ȡ��ַ
	printf("#define Base_NearObjList 0x%08X		//��Χ�����б��ַ\n", ndBaseAddr);
	//Base_PlayerObj
	strcpy_s(szpFeatureCode, "85C074**83B8880100000074**8D8D30FCFFFF");
	ndLocateAddr = ScanFeatureCode(hProcess, szpFeatureCode);	//��λ������
	ndBaseAddr = ReadBaseAddr(hProcess, ndLocateAddr -4);		//��ȡ��ַ
	printf("#define Base_PlayerObj 0x%08X		//��Ҷ����ַ\n\n", ndBaseAddr);

	//Base_Unknown
	strcpy_s(szpFeatureCode, "6A09E8********5F5E5B8BE55DC2");
	ndLocateAddr = ScanFeatureCode(hProcess, szpFeatureCode);	//��λ������
	ndBaseAddr = ReadBaseAddr(hProcess, ndLocateAddr -4);		//��ȡ��ַ
	printf("#define Base_Unknown 0x%08X			//δ֪�����ַ\n", ndBaseAddr);
	//Base_Unknown2
	strcpy_s(szpFeatureCode, "8B15********3BC274**8B083BCB");
	ndLocateAddr = ScanFeatureCode(hProcess, szpFeatureCode);	//��λ������
	ndBaseAddr = ReadBaseAddr(hProcess, ndLocateAddr - 4);		//��ȡ��ַ
	printf("#define Base_Unknown2 0x%08X		//δ֪�����ַ2\n\n", ndBaseAddr);

	//Base_ActionList
	strcpy_s(szpFeatureCode, "833C0600743C8B04068B50548B7858");
	ndLocateAddr = ScanFeatureCode(hProcess, szpFeatureCode);	//��λ������
	ndBaseAddr = ReadBaseAddr(hProcess, ndLocateAddr -4);		//��ȡ��ַ
	printf("#define Base_ActionList 0x%08X			//�����б��ַ\n", ndBaseAddr);
	//Call_ActionUse
	strcpy_s(szpFeatureCode, "83BF341600003675**8B84B73C04000085C0");
	ndLocateAddr = ScanFeatureCode(hProcess, szpFeatureCode);	//��λ������
	ndBaseAddr = ReadBaseAddr(hProcess, ndLocateAddr - 4);		//��ȡ��ַ
	printf("#define Call_ActionUse 0x%08X		//����ʹ��CALL\n\n", ndBaseAddr+ ndLocateAddr);

	//Base_SkillList 
	strcpy_s(szpFeatureCode, "33C0A3********8B96C00B000083C2288950388B86C00B00008B0D");
	ndLocateAddr = ScanFeatureCode(hProcess, szpFeatureCode);	//��λ������
	ndBaseAddr = ReadBaseAddr(hProcess, ndLocateAddr +3);		//��ȡ��ַ
	printf("#define Base_SkillList  0x%08X			//�����б��ַ\n", ndBaseAddr);
	//Call_LearnSkill
	strcpy_s(szpFeatureCode, "8B94B78C0300008B0D********8B425451508D8DF4D7FFFF518B0D");
	ndLocateAddr = ScanFeatureCode(hProcess, szpFeatureCode);	//��λ������
	ndBaseAddr = ReadBaseAddr(hProcess, ndLocateAddr +32);		//��ȡ��ַ
	printf("#define Call_LearnSkill 0x%08X		//��������CALL\n", ndBaseAddr+ ndLocateAddr+36);
	//Call_LearnSkill_Ecx
	strcpy_s(szpFeatureCode, "5068********8D4DBC6A4051E8********8B8E2C0F0000");
	ndLocateAddr = ScanFeatureCode(hProcess, szpFeatureCode);	//��λ������
	ndBaseAddr = ReadBaseAddr(hProcess, ndLocateAddr - 4);		//��ȡ��ַ
	printf("#define Call_LearnSkill_Ecx 0x%08X	//��������CALL��ECX\n\n", ndBaseAddr);

	//Base_ShortcutBar
	strcpy_s(szpFeatureCode, "33C08D8E3C040000EB**8D490083390074");
	ndLocateAddr = ScanFeatureCode(hProcess, szpFeatureCode);	//��λ������
	ndBaseAddr = ReadBaseAddr(hProcess, ndLocateAddr - 4);		//��ȡ��ַ
	printf("#define Base_ShortcutBar 0x%08X			//�������ַ\n", ndBaseAddr);
	//Call_F1F10Use
	strcpy_s(szpFeatureCode, "83BC8A3C040000000F84********A16C452501518B888C020000E8");
	ndLocateAddr = ScanFeatureCode(hProcess, szpFeatureCode);	//��λ������
	ndBaseAddr = ReadBaseAddr(hProcess, ndLocateAddr +27);		//��ȡ��ַ
	printf("#define Call_F1F10Use 0x%08X		//�����ʹ��CALL\n\n", ndBaseAddr + ndLocateAddr + 31);

	//Base_MouseSelObj
	strcpy_s(szpFeatureCode, "85D274**833C070074**8B0C078B59588B49548B400C");
	ndLocateAddr = ScanFeatureCode(hProcess, szpFeatureCode);	//��λ������
	ndBaseAddr = ReadBaseAddr(hProcess, ndLocateAddr - 4);		//��ȡ��ַ
	printf("#define Base_MouseSelObj 0x%08X			//�������Ķ����ַ\n", ndBaseAddr);
	printf("											//�������Ķ���:[[�������Ķ����ַ]+0x230]\n");
	//Call_MoveObjToList
	strcpy_s(szpFeatureCode, "8B86900200008B0D********8B9134160000508B81601C00005250E8");
	ndLocateAddr = ScanFeatureCode(hProcess, szpFeatureCode);	//��λ������
	ndBaseAddr = ReadBaseAddr(hProcess, ndLocateAddr + 28);		//��ȡ��ַ
	printf("#define Call_MoveObjToList 0x%08X	//�ƶ������б�CALL\n\n", ndBaseAddr + ndLocateAddr + 32);

	//Call_ChatWithNPC
	strcpy_s(szpFeatureCode, "33C05DC20C008B5510528B550C5250E8");
	ndLocateAddr = ScanFeatureCode(hProcess, szpFeatureCode);	//��λ������
	ndBaseAddr = ReadBaseAddr(hProcess, ndLocateAddr + 16);		//��ȡ��ַ
	printf("#define Call_ChatWithNPC 0x%08X			//��NPC�Ի�CALL\n", ndBaseAddr + ndLocateAddr + 20);
	//Call_ClickChatMenuOption
	strcpy_s(szpFeatureCode, "8B9C9EA801000081FB3C0100000F8D********538BCEE8");
	ndLocateAddr = ScanFeatureCode(hProcess, szpFeatureCode);	//��λ������
	ndBaseAddr = ReadBaseAddr(hProcess, ndLocateAddr + 23);		//��ȡ��ַ
	printf("#define Call_ClickChatMenuOption 0x%08X	//����Ի��˵�ѡ��CALL\n\n", ndBaseAddr + ndLocateAddr + 27);

	//Base_ViewRange
	strcpy_s(szpFeatureCode, "6A016A006A006A006A0068********525056E8********D905");
	ndLocateAddr = ScanFeatureCode(hProcess, szpFeatureCode);	//��λ������
	ndBaseAddr = ReadBaseAddr(hProcess, ndLocateAddr + 25);		//��ȡ��ַ
	printf("#define Base_ViewRange 0x%08X			//��ҿ��ӷ�Χ��ַ\n\n", ndBaseAddr);

	//Base_ScreenRefreshHook
	strcpy_s(szpFeatureCode, "8B86940100008B088B91A800000050FFD28BCE");
	ndLocateAddr = ScanFeatureCode(hProcess, szpFeatureCode);	//��λ������
	printf("#define Base_ScreenRefreshHook 0x%08X		//����ˢ��Hook��ַ\n\n", ndLocateAddr + 14);
}
