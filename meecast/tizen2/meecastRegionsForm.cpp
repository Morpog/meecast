/* vim: set sw=4 ts=4 et: */
/*
 * This file is part of Meecast for Tizen2
 *
 * Copyright (C) 2013 Vlad Vasilyeu
 *     for the code
 *
 * This software is free software; you can redistribute it and/or
 * modify it under the terms of the GNU  General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 *  General Public License for more details.
 *
 * You should have received a copy of the GNU  General Public
 * License along with this software; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
*/
/*******************************************************************************/
#include <FApp.h>
#include <FIo.h>
#include "meecastRegionsForm.h"

using namespace Tizen::Base;
using namespace Tizen::App;
using namespace Tizen::Ui;
using namespace Tizen::Ui::Controls;
using namespace Tizen::Ui::Scenes;
using namespace Tizen::Base;
using namespace Tizen::Base::Collection;
using namespace Tizen::Io;

static const int LIST_HEIGHT = 112;
static const int BUTTON_HEIGHT = 74;

meecastRegionsForm::meecastRegionsForm(void)
                    : __pListView(null)
{
}

meecastRegionsForm::~meecastRegionsForm(void)
{
    __map->RemoveAll(true);
}

bool
meecastRegionsForm::Initialize(void)
{
    Construct(L"COUNTRIES_FORM");
    return true;
}

result
meecastRegionsForm::OnInitializing(void)
{
    result r = E_SUCCESS;

    // TODO:
    // Add your initialization code here

    //CreateContextMenuList();
    // Setup back event listener
    SetFormBackEventListener(this);


    // Creates an instance of ListView
    __pListView = new ListView();
    __pListView->Construct(Tizen::Graphics::Rectangle(0, 0, GetClientAreaBounds().width, GetClientAreaBounds().height), true, false);
    __pListView->SetItemProvider(*this);
    __pListView->AddListViewItemEventListener(*this);

    // Adds the list view to the form
    AddControl(*__pListView);

    return r;
}

result
meecastRegionsForm::OnTerminating(void)
{
    result r = E_SUCCESS;

    // TODO:
    // Add your termination code here
    return r;
}

void
meecastRegionsForm::OnActionPerformed(const Tizen::Ui::Control& source, int actionId)
{
    SceneManager* pSceneManager = SceneManager::GetInstance();
    AppAssert(pSceneManager);

    AppLog("Check Action");
    switch(actionId)
    {
    default:
        break;
    }
}

void
meecastRegionsForm::OnFormBackRequested(Tizen::Ui::Controls::Form& source)
{
	SceneManager* pSceneManager = SceneManager::GetInstance();
	AppAssert(pSceneManager);

	pSceneManager->GoBackward(BackwardSceneTransition(SCENE_TRANSITION_ANIMATION_TYPE_RIGHT));

}

void
meecastRegionsForm::OnSceneActivatedN(const Tizen::Ui::Scenes::SceneId& previousSceneId,
                                          const Tizen::Ui::Scenes::SceneId& currentSceneId, Tizen::Base::Collection::IList* pArgs)
{
    AppLog("OnSceneActivatedN Regions");
    if (pArgs != null) {
        __db = static_cast < Core::DatabaseSqlite* > (pArgs->GetAt(0));
        __SourceId = *(dynamic_cast<String*> (pArgs->GetAt(1)));
        __CountryName = *(dynamic_cast<String*> (pArgs->GetAt(2)));
        pArgs->RemoveAt(2);
        pArgs->RemoveAt(1);
        AppLog("SOurceID %S", __SourceId.GetPointer());
        delete pArgs;
        LoadList();
    }
}

void
meecastRegionsForm::OnSceneDeactivated(const Tizen::Ui::Scenes::SceneId& currentSceneId,
                                           const Tizen::Ui::Scenes::SceneId& nextSceneId)
{
    // TODO:
    // Add your scene deactivate code here
    AppLog("OnSceneDeactivated");
}

int
meecastRegionsForm::GetItemCount(void)
{
    int itemCount = 0;

    IList* pList = __map->GetKeysN();
    AppAssert(pList);
    itemCount = pList->GetCount();
    delete pList;
    return itemCount;
}

bool
meecastRegionsForm::DeleteItem(int index, Tizen::Ui::Controls::ListItemBase* pItem, int itemWidth)
{
	delete pItem;
	return true;
}


Tizen::Ui::Controls::ListItemBase*
meecastRegionsForm::CreateItem (int index, int itemWidth)
{
    CustomItem* pItem = new (std::nothrow) CustomItem();
    TryReturn(pItem != null, null, "Out of memory");

    pItem->Construct(Tizen::Graphics::Dimension(itemWidth, LIST_HEIGHT), LIST_ANNEX_STYLE_NORMAL);

    String* pStr = dynamic_cast< String* >(__map->GetValue(Integer(index)));
    pItem->AddElement(Tizen::Graphics::Rectangle(26, 32, 600, 50), 0, *pStr, false);
    return pItem;
}

void
meecastRegionsForm::OnListViewItemStateChanged(Tizen::Ui::Controls::ListView& listView, int index, int elementId, Tizen::Ui::Controls::ListItemStatus status)
{
	if (status == LIST_ITEM_STATUS_SELECTED)
	{
        SceneManager* pSceneManager = SceneManager::GetInstance();
        AppAssert(pSceneManager);

        Tizen::Base::Collection::ArrayList* pList = new Tizen::Base::Collection::ArrayList();
        AppAssert(pList);
        pList->Construct();
        pList->Add(*__db);

        String* tempS = new String("");
        tempS->Append(__SourceId);
        String* pStr0 = dynamic_cast< String* >(tempS);
        pList->Add(*(new String(*pStr0)));
        tempS->Clear();
        tempS->Append(__CountryName);
        pStr0 = dynamic_cast< String* >(tempS);
        pList->Add(*(new String(*pStr0)));
        delete tempS;

        String* pStr = dynamic_cast< String* >(__map->GetValue(Integer(index)));
        pList->Add(*(new String(*pStr)));
        
        pSceneManager->GoForward(SceneTransitionId(L"ID_SCNT_STATIONSSCENE"), pList);
	    AppLog("LIST_ITEM_STATUS_SELECTED ");


   	}
}
void
meecastRegionsForm::OnListViewItemSwept(Tizen::Ui::Controls::ListView& listView, int index, Tizen::Ui::Controls::SweepDirection direction)
{
}

void
meecastRegionsForm::OnListViewContextItemStateChanged(Tizen::Ui::Controls::ListView& listView, int index, int elementId, Tizen::Ui::Controls::ListContextItemStatus state)
{
}

void
meecastRegionsForm::OnItemReordered(Tizen::Ui::Controls::ListView& view, int oldIndex, int newIndex)
{
}

bool
meecastRegionsForm::LoadList(void){
    AppLog("Open DB success");
     __map = __db->create_region_list_by_name(__CountryName);
    return true;
}


