#pragma once
//��ַ����Ԫ
#define Base_GameWndHandle 0x01196F88	//��Ϸ�����ھ��

#define Base_AllObjList 0x02E65A28		//���ж����б��ַ
										//�ض������ַ��[���ж����б��ַ+4*ID]

#define Base_RoleProperty 0x02C186D8		//�������Ի�ַ

#define Base_EquipList 0x02E3D3E0			//װ���б��ַ
#define Base_ShopList 0x0119B7B0			//�̵��б��ַ
#define Base_DepotList 0x02E4692C			//�ֿ��б��ַ
#define Base_BackPackList 0x02E3D3E4		//�����б��ַ
#define Call_UseObjForIndex 0x00838EA0	//������Ʒʹ��CALL
#define Call_SendData 0x004F8740		//��������CALL
#define Call_SendData_Ecx 0x01197000	//��������CALL��Ecx

#define Base_NearObjList 0x04280BA0		//��Χ�����б��ַ
#define Base_PlayerObj 0x02E65A24		//��Ҷ����ַ

#define Base_Unknown 0x0125456C			//δ֪�����ַ
#define Base_Unknown2 0x02C1EA6C		//δ֪�����ַ2

#define Base_ActionList 0x02E3DD58			//�����б��ַ
#define Call_ActionUse 0x00714220		//����ʹ��CALL

#define Base_SkillList  0x02E3EF64			//�����б��ַ
#define Call_LearnSkill 0x00555B80		//��������CALL
#define Call_LearnSkill_Ecx 0x02C18784	//��������CALL��ECX

#define Base_ShortcutBar 0x02E3EF08			//�������ַ
#define Call_F1F10Use 0x00723420		//�����ʹ��CALL

#define Base_MouseSelObj 0x0125455C			//�������Ķ����ַ
											//�������Ķ���:[[�������Ķ����ַ]+0x230]
#define Call_MoveObjToList 0x0084A180	//�ƶ������б�CALL

#define Call_ChatWithNPC 0x0050D3F0			//��NPC�Ի�CALL
#define Call_ClickChatMenuOption 0x007D5B30	//����Ի��˵�ѡ��CALL

#define Base_ViewRange 0x00AA1DBC			//��ҿ��ӷ�Χ��ַ

#define Base_ScreenRefreshHook 0x00592CE2			//����ˢ��Hook��ַ

#define Call_ActionUse_Ecx *(DWORD*)(*(DWORD*)(Base_Unknown)+0x28C)
#define Call_F1F10Use_Ecx *(DWORD*)(*(DWORD*)(Base_Unknown)+0x28C)
