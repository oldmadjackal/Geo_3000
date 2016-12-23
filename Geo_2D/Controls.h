/*--------------------------------------------------------- LIST BOX */

#define  LB_CLEAR(elm)               SendMessage(GetDlgItem(hDlg, elm), \
					          LB_RESETCONTENT ,     \
						    0, 0)
#define  LB_ADD_ROW(elm, text)       SendMessage(GetDlgItem(hDlg, elm), \
					           LB_ADDSTRING,        \
						    0, (LPARAM)text)
#define  LB_SET_ITEM(elm, num, ptr)  SendMessage(GetDlgItem(hDlg, elm), \
					           LB_SETITEMDATA,      \
						    (WPARAM)num,        \
                                                      (LPARAM)ptr)
#define  LB_DEL_ROW(elm, num)        SendMessage(GetDlgItem(hDlg, elm), \
					           LB_DELETESTRING,     \
						    (WPARAM)num, 0)
#define  LB_GET_ROW(elm)             SendMessage(GetDlgItem(hDlg, elm), \
                                                   LB_GETCURSEL, 0, 0)
#define  LB_GET_ITEM(elm)            SendMessage(GetDlgItem(hDlg, elm), \
					           LB_GETITEMDATA,      \
                                      SendMessage(GetDlgItem(hDlg, elm), \
                                                    LB_GETCURSEL, 0, 0), \
                                                         0)
#define  LB_ROW_ITEM(elm, num)        SendMessage(GetDlgItem(hDlg, elm), \
					           LB_GETITEMDATA,       \
						    (WPARAM)num, 0)
                                                         
/*------------------------------------------------------------- EDIT */

#define  SETs(elm, s)  SetDlgItemText(hDlg, elm, s)
#define  GETs(elm, s)  GetDlgItemText(hDlg, elm, s, 512)

/*-------------------------------------------------------- CHECK BOX */

#define  SETx(elm, v) CheckDlgButton(hDlg, elm, (v>0)?BST_CHECKED:BST_UNCHECKED)
#define  GETx(elm)    ((IsDlgButtonChecked(hDlg, elm)==BST_CHECKED)?1:0)

/*------------------------------------------------------------ TOOLS */

#define  ENABLE(elm)   EnableWindow(GetDlgItem(hDlg, elm), TRUE)
#define  DISABLE(elm)  EnableWindow(GetDlgItem(hDlg, elm), FALSE)

#define  ITEM(e)  GetDlgItem(hDlg, e)
