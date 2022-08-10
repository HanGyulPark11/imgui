#include "Application.h"

#include "imgui.h"
#include "player.h"
#include "Land.h"
#include <stdio.h>
#include "goldKey.h"

namespace MyApp
{
    void RenderUI()
    {
        static bool opt_fullscreen = true;
        static bool opt_padding = false;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
        static int landPrice = 20;
        static int hotelPrice = 50;
        static int landToll = 30;
        static int hotelFee = 100;
        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen)
        {
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }
        else
        {
            dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
        }

        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
        // and handle the pass-thru hole, so we ask Begin() to not render a background.
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
        // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
        // all active windows docked into it will lose their parent and become undocked.
        // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
        // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
        if (!opt_padding)
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace Demo", nullptr, window_flags);
        if (!opt_padding)
            ImGui::PopStyleVar();

        if (opt_fullscreen)
            ImGui::PopStyleVar(2);

        // Submit the DockSpace
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Options"))
            {
                // Disabling fullscreen would allow the window to be moved to the front of other windows,
                // which we can't undo at the moment without finer window depth/z control.
                ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
                ImGui::MenuItem("Padding", NULL, &opt_padding);
                ImGui::Separator();

                if (ImGui::MenuItem("Flag: NoSplit", "", (dockspace_flags & ImGuiDockNodeFlags_NoSplit) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoSplit; }
                if (ImGui::MenuItem("Flag: NoResize", "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoResize; }
                if (ImGui::MenuItem("Flag: NoDockingInCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingInCentralNode) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoDockingInCentralNode; }
                if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar; }
                if (ImGui::MenuItem("Flag: PassthruCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0, opt_fullscreen)) { dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode; }
                ImGui::Separator();

                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
        static bool gameOn = false;
        static int noPlayers = 4;
        static int turn = 0;
        static bool alpha_preview = true;
        static bool alpha_half_preview = false;
        static bool drag_and_drop = true;
        static bool options_menu = true;
        static bool hdr = false;
        ImGui::Checkbox("With Alpha Preview", &alpha_preview);
        ImGui::Checkbox("With Half Alpha Preview", &alpha_half_preview);
        ImGui::Checkbox("With Drag and Drop", &drag_and_drop);
        ImGui::Checkbox("With Options Menu", &options_menu);
        ImGui::Checkbox("With HDR", &hdr); ImGui::SameLine();
        ImGuiColorEditFlags misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);


        static ImVec4 color1 = ImVec4(255.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f, 255.0f / 255.0f);
        static ImVec4 color2 = ImVec4(255.0f / 255.0f, 255.0f / 255.0f, 0.0f / 255.0f, 255.0f / 255.0f);
        static ImVec4 color3 = ImVec4(0.0f / 255.0f, 255.0f / 255.0f, 0.0f / 255.0f, 255.0f / 255.0f);
        static ImVec4 color4 = ImVec4(0.0f / 255.0f, 0.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f);
        static ImVec4 lcolor;
        static ImVec2 size(100.0f, 100.0f);
        static ImVec2 offset(20.0f, 0.0f);
        static ImVec2 offset2(20.0f, 80.0f);
        static ImVec4 noColor = ImVec4(90, 90, 120, 255);
        static ImVec4 colf = ImVec4(90, 90, 120, 255);


        const ImU32 col = ImColor(colf);
        static bool circle_segments_override = false;
        static int circle_segments_override_v = 12;
        const int circle_segments = circle_segments_override ? circle_segments_override_v : 0;
        static int ngon_sides = 6;
        static float sz = 36.0f;
        static float sz2 = 90.0f;
        float x = 1430.0f;
        float y = 1000.0f;
        const float spacing = 10.0f;
        static int gNum = 10;
        static int g = 0;
        ImGui::Begin("Game");
        if (!gameOn)
        {
            if (ImGui::Button("Game Start"))
                gameOn = true;
        }
        else {

            static GoldKey* gold = new GoldKey[10];
            for (int n = 0; n < 10; n++) {
                gold[n] = GoldKey();
                switch (n)
                {
                case 0:
                    gold[n].setDescription("Use This Card To Escape Jail Immediately");
                    break;
                case 1:
                    gold[n].setDescription("Use This Card Instead Of Paying Toll");
                    break;
                case 2:
                    gold[n].setDescription("Move Backwards Twice");
                    gold[n].setUsedWhenDrawn();
                    break;
                case 3:
                    gold[n].setDescription("MoveBackwards Three Times");
                    gold[n].setUsedWhenDrawn();
                    break;
                case 4:
                    gold[n].setDescription("Go To Korea");
                    gold[n].setUsedWhenDrawn();
                    break;
                case 5:
                    gold[n].setDescription("Go To Germany");
                    gold[n].setUsedWhenDrawn();
                    break;
                case 6:
                    gold[n].setDescription("Slow Down! You Were Fined For $100");
                    gold[n].setUsedWhenDrawn();
                    break;
                case 7:
                    gold[n].setDescription("Go To World Travel");
                    gold[n].setUsedWhenDrawn();
                    break;
                case 8:
                    gold[n].setDescription("Go To Jail");
                    gold[n].setUsedWhenDrawn();
                    break;
                case 9:
                    gold[n].setDescription("Won Lottery! Got $500");
                    gold[n].setUsedWhenDrawn();
                    break;
                }
            }
            ImDrawList* draw_list = ImGui::GetWindowDrawList();
            static Land* land = new Land[32];
            //draw_list->AddNgonFilled(ImVec2(x + sz * 0.5f, y + sz * 0.5f), sz * 0.5f, col, ngon_sides);               x += sz + spacing;  // N-gon
            //draw_list->AddCircleFilled(ImVec2(x + sz * 0.5f, y + sz * 0.5f), sz * 0.5f, col, circle_segments);            x += sz + spacing;  // Circle
            //draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + sz, y + sz), col);                                    x += sz + spacing;  // Square


            for (int n = 0; n < 32; n++) {
                //ImGui::PushID(n);
                //ImGui::InvisibleButton("##canvas", size);
                //ImGui::PopID();
                //if (!ImGui::IsItemVisible()) // Skip rendering as ImDrawList elements are not clipped.
                //    continue;

                const ImVec2 text_pos = ImVec2(x - sz2, y - sz2 + offset.y);

                
                land[n] = Land(n, ImVec2(x - sz2, y - sz2));
                

                //ImGui::PushClipRect(x, y, true);
                draw_list->AddRectFilled(ImVec2(x - sz2, y - sz2), ImVec2(x, y), IM_COL32(90, 90, 120, 255));

                switch (n)
                {
                case 0:
                    draw_list->AddText(text_pos, IM_COL32_WHITE, "Start");
                    land[0].setName("Start");
                    land[0].Special();
                    break;
                case 1:
                    draw_list->AddText(text_pos, IM_COL32_WHITE, "Vietnam\n\nHotels");
                    land[1].setName("Vietnam");
                    break;
                case 2:
                    draw_list->AddText(text_pos, IM_COL32_WHITE, "India\n\nHotels");
                    land[2].setName("India");
                    break;
                case 3:
                    draw_list->AddText(text_pos, IM_COL32_WHITE, "China\n\nHotels");
                    land[3].setName("China");
                    break;
                case 4:
                    draw_list->AddText(text_pos, IM_COL32_WHITE, "Donation");
                    land[4].setName("Donation");
                    land[4].Special();
                    break;
                case 5:
                    draw_list->AddText(text_pos, IM_COL32_WHITE, "UAE\n\nHotels");
                    land[5].setName("UAE");
                    break;
                case 6:
                    draw_list->AddText(text_pos, IM_COL32_WHITE, "Egypt\n\nHotels");
                    land[6].setName("Egypt");
                    break;
                case 7:
                    draw_list->AddText(text_pos, IM_COL32_WHITE, "Turkey\n\nHotels");
                    land[7].setName("Turkey");
                    break;
                case 8:
                    draw_list->AddText(text_pos, IM_COL32_WHITE, "Jail");
                    land[8].setName("Jail");
                    land[8].Special();
                    break;
                case 9:
                    draw_list->AddText(text_pos, IM_COL32_WHITE, "Japan\n\nHotels");
                    land[9].setName("Japan");
                    break;
                case 10:
                    draw_list->AddText(text_pos, IM_COL32_WHITE, "Mexico\n\nHotels");
                    land[10].setName("Mexico");
                    break;
                case 11:
                    draw_list->AddText(text_pos, IM_COL32_WHITE, "Brazil\n\nHotels");
                    land[11].setName("Brazil");
                    break;
                case 12:
                    draw_list->AddText(text_pos, IM_COL32_WHITE, "Gold Key");
                    land[12].setName("Gold Key");
                    land[12].Special();
                    break;
                case 13:
                    draw_list->AddText(text_pos, IM_COL32_WHITE, "Australia\n\nHotels");
                    land[13].setName("Australia");
                    break;
                case 14:
                    draw_list->AddText(text_pos, IM_COL32_WHITE, "Denmark\n\nHotels");
                    land[14].setName("Denmark");
                    break;
                case 15:
                    draw_list->AddText(text_pos, IM_COL32_WHITE, "Spain\n\nHotels");
                    land[15].setName("Spain");
                    break;
                case 16:
                    draw_list->AddText(text_pos, IM_COL32_WHITE, "To Start");
                    land[16].setName("To Start");
                    land[16].Special();
                    break;
                case 17:
                    draw_list->AddText(text_pos, IM_COL32_WHITE, "Sweden\n\nHotels");
                    land[17].setName("Sweden");
                    break;
                case 18:
                    draw_list->AddText(text_pos, IM_COL32_WHITE, "Norway\n\nHotels");
                    land[18].setName("Norway");
                    break;
                case 19:
                    draw_list->AddText(text_pos, IM_COL32_WHITE, "Germany\n\nHotels");
                    land[19].setName("Germany");
                    break;
                case 20:
                    draw_list->AddText(text_pos, IM_COL32_WHITE, "Lucky!");
                    land[20].setName("Lucky!");
                    land[20].Special();
                    break;
                case 21:
                    draw_list->AddText(text_pos, IM_COL32_WHITE, "Russia\n\nHotels");
                    land[21].setName("Russia");
                    break;
                case 22:
                    draw_list->AddText(text_pos, IM_COL32_WHITE, "Italy\n\nHotels");
                    land[22].setName("Italy");
                    break;
                case 23:
                    draw_list->AddText(text_pos, IM_COL32_WHITE, "Netherlands\n\nHotels");
                    land[23].setName("Netherlands");
                    break;
                case 24:
                    draw_list->AddText(text_pos, IM_COL32_WHITE, "World Travel!");
                    land[24].setName("World Travel!");
                    land[24].Special();
                    break;
                case 25:
                    draw_list->AddText(text_pos, IM_COL32_WHITE, "Canada\n\nHotels");
                    land[25].setName("Canada");
                    break;
                case 26:
                    draw_list->AddText(text_pos, IM_COL32_WHITE, "UK\n\nHotels");
                    land[26].setName("UK");
                    break;
                case 27:
                    draw_list->AddText(text_pos, IM_COL32_WHITE, "France\n\nHotels");
                    land[27].setName("France");
                    break;
                case 28:
                    draw_list->AddText(text_pos, IM_COL32_WHITE, "Gold Key");
                    land[28].setName("Gold Key");
                    land[28].Special();
                    break;
                case 29:
                    draw_list->AddText(text_pos, IM_COL32_WHITE, "USA\n\nHotels");
                    land[29].setName("USA");
                    break;
                case 30:
                    draw_list->AddText(text_pos, IM_COL32_WHITE, "Korea\n\nHotels");
                    land[30].setName("Korea");
                    break;
                case 31:
                    draw_list->AddText(text_pos, IM_COL32_WHITE, "Bank\n\nHotels");
                    land[31].setName("Bank");
                    land[31].Special();
                    break;
                }

                //ImGui::PopClipRect();
                /*if (0 <= n <= 7) {
                    x -= sz2 + spacing;
                }
                if (16 <= n <= 23) {
                    x += sz2 + spacing;
                }
                if (8 <= n <= 15) {
                    y -= sz2 + spacing;                 이케하면안댐
                }
                if (24 <= n <= 31) {
                    y += sz2 + spacing;
                }*/

                if (0 <= n && n <= 7) {
                    x -= sz2 + spacing;
                }
                if (16 <= n && n <= 23) {
                    x += sz2 + spacing;
                }
                if (8 <= n && n <= 15) {
                    y -= sz2 + spacing;
                }
                if (24 <= n && n <= 31) {
                    y += sz2 + spacing;
                }
            }
            static Player player1 = Player();
            player1.setInd(1);

            static Player player2 = Player();
            player2.setInd(2);

            static Player player3 = Player();
            player3.setInd(3);

            static Player player4 = Player();
            player1.setInd(4);


            static int d11, d12;
            ImGui::Begin("Player 1");
            if (player1.isBroke()) {
                ImGui::BulletText("You are broke");
                if (turn==0)
                    turn = (turn + 1) % 4;
            }
            else
            {
                ImGui::BulletText("Money : $%d", player1.getMoney());
                ImGui::BulletText("Position : %d", player1.getPos());
                if (player1.jailTurn() > 0) {
                    ImGui::BulletText("in jail for %d turns", player1.jailTurn());
                }
                if (player1.getTurns() > 0) {
                    ImGui::BulletText("Debt is due in %d turns", player1.getTurns());
                }
                else if (player1.getTurns() == 0) {
                    ImGui::BulletText("Debt has been repaid");
                }
                if (player1.usedKey() != -1) {
                    ImGui::BulletText("%s", gold[player1.usedKey()].getDescription());
                }
                ImGui::BulletText("Assets :");
                int* Assets1 = player1.getAssets();
                static Player* owner1 = nullptr;
                static int toll1 = 0;
                for (int i = 0; i < 32; i++) {
                    printf("%d %d\n", i, Assets1[i]);
                    if (Assets1[i] > 0) {
                        ImGui::Text("\t%d hotels in %s", Assets1[i] - 1, land[i].getName());
                    }
                }

                ImGui::Text("index is %d", player1.getInd());
                if (owner1 != nullptr) {
                    ImGui::Text("owner is %d", owner1->getInd());
                }
            
                if (turn != 0)
                    ImGui::Button("Done");
                else if (player1.isBusy()&&!land[player1.getPos()].isSpecial()) {
                    int pos = player1.getPos();
                    bool owned = player1.getAssets()[pos] || player2.getAssets()[pos] || player3.getAssets()[pos] || player4.getAssets()[pos];
                    if (!owned) {
                        static int i1 = 0;
                        ImGui::SliderInt("slider int", &i1, 0, 3);

                        static int selected = -1;
                        const char* names[] = { "Yes", "No" };
                        if (ImGui::Button("Buy"))
                            ImGui::OpenPopup("my_select_popup");
                        ImGui::SameLine();
                        ImGui::TextUnformatted(selected == -1 ? "<None>" : names[selected]);
                        if (ImGui::BeginPopup("my_select_popup"))
                        {
                            ImGui::Text("It will cost $%d", landPrice + hotelPrice * i1);
                            ImGui::Separator();
                            for (int i = 0; i < IM_ARRAYSIZE(names); i++)
                                if (ImGui::Selectable(names[i]))
                                    selected = i;
                            ImGui::EndPopup();
                        }
                        if (selected == 0) {
                            static Player* pt1 = &player1;
                            if (player1.buy(player1.getPos(), i1)) {
                                land[player1.getPos()].setPlayer(pt1);
                                land[player1.getPos()].setColor(color1);
                            }
                            player1.unlock();
                        }
                        else if (selected == 1) {
                            player1.unlock();
                        }
                        selected = -1;
                    }
                    else if (player1.getAssets()[pos] > 0) {
                        static int i1 = 0;

                        ImGui::SliderInt("slider int", &i1, 0, 3 - player1.getAssets()[pos] + 1);

                        static int selected = -1;
                        const char* names[] = { "Yes", "No" };
                        if (ImGui::Button("Buy"))
                            ImGui::OpenPopup("my_select_popup");
                        ImGui::SameLine();
                        ImGui::TextUnformatted(selected == -1 ? "<None>" : names[selected]);
                        if (ImGui::BeginPopup("my_select_popup"))
                        {
                            ImGui::Text("It will cost $%d", hotelPrice * i1);
                            ImGui::Separator();
                            for (int i = 0; i < IM_ARRAYSIZE(names); i++)
                                if (ImGui::Selectable(names[i]))
                                    selected = i;
                            ImGui::EndPopup();
                        }
                        if (selected == 0) {
                            static Player* pt1 = &player1;
                            if (player1.buy(player1.getPos(), i1)) {
                                land[player1.getPos()].setPlayer(pt1);
                            }
                            player1.unlock();
                        }
                        else if (selected == 1) {
                            player1.unlock();
                        }
                        selected = -1;
                    }
                    else if (player2.getAssets()[pos] > 0) {
                        int h = player2.getAssets()[pos];
                        toll1 = player1.pay(&player2, h - 1);
                        if (toll1>0) 
                        {
                            owner1 = &player2;
                        }
                        player1.unlock();
                    }
                    else if (player3.getAssets()[pos] > 0) {
                        int h = player3.getAssets()[pos];
                        toll1 = player1.pay(&player3, h - 1);
                        if (toll1 > 0)
                        {
                            owner1 = &player3;
                        }
                        player1.unlock();
                    }
                    else if (player4.getAssets()[pos] > 0) {
                        int h = player4.getAssets()[pos];
                        toll1 = player1.pay(&player4, h - 1);
                        if (toll1 > 0)
                        {
                            owner1 = &player4;
                        }
                        player1.unlock();
                    }
                }
                else if (player1.isBusy() && land[player1.getPos()].isSpecial()) {
                    int p1 = player1.getPos();
                    switch (p1)
                    {
                    case 0:
                        player1.unlock();
                        break;
                    case 4:
                        player1.addMoney(-100);
                        player1.unlock();
                        break;
                    case 8:
                        if (player1.jailTurn() < 0) {
                            player1.Jail();
                        }
                        player1.unlock();
                        break;
                    case 12:
                        if (gNum == 0) {
                            break;
                        }
                        g = player1.drawGoldKey(gold, gNum);
                        gNum--;

                        switch (g)
                        {
                        case 0:
                            player1.unlock();
                            break;
                        case 1:
                            player1.unlock();
                            break;
                        case 2:
                            player1.backwards(2);
                            break;
                        case 3:
                            player1.backwards(3);
                            break;
                        case 4:
                            player1.proceed((30 - player1.getPos()) % 32);
                            break;
                        case 5:
                            player1.proceed((19 - player1.getPos()) % 32);
                            break;
                        case 6:
                            player1.addMoney(-100);
                            player1.unlock();
                            break;
                        case 7:
                            player1.proceed((24 - player1.getPos()) % 32);
                            break;
                        case 8:
                            player1.proceed((8 - player1.getPos()) % 32);
                            break;
                        case 9:
                            player1.addMoney(500);
                            player1.unlock();
                            break;
                        }
                    case 16:
                        player1.proceed(16);
                        player1.unlock();
                        break;
                    case 20:
                        player1.addMoney(200);
                        player1.unlock();
                        break;
                    case 24:
                    {
                        static int i1 = 0;
                        ImGui::SliderInt("slider int", &i1, 0, 32);

                        static int selected = -1;
                        const char* names[] = { "Yes", "No" };
                        if (ImGui::Button("Go"))
                            ImGui::OpenPopup("my_select_popup");
                        ImGui::SameLine();
                        ImGui::TextUnformatted(selected == -1 ? "<None>" : names[selected]);
                        if (ImGui::BeginPopup("my_select_popup"))
                        {
                            int go = (p1 + i1) % 32;
                            ImGui::Text("Going to %s", land[go].getName());
                            ImGui::Separator();
                            for (int i = 0; i < IM_ARRAYSIZE(names); i++)
                                if (ImGui::Selectable(names[i]))
                                    selected = i;
                            ImGui::EndPopup();
                        }
                        if (selected == 0) {
                            player1.proceed(i1);
                        }
                        selected = -1;
                        break;
                    }
                    case 28:
                        if (gNum == 0) {
                            break;
                        }
                        g=player1.drawGoldKey(gold, gNum);
                        gNum--;

                        switch (g)
                        {
                        case 0:
                            player1.unlock();
                            break;
                        case 1:
                            player1.unlock();
                            break;
                        case 2:
                            player1.backwards(2);
                            break;
                        case 3:
                            player1.backwards(3);
                            break;
                        case 4:
                            player1.proceed((30 - player1.getPos()) % 32);
                            break;
                        case 5:
                            player1.proceed((19 - player1.getPos()) % 32);
                            break;
                        case 6:
                            player1.addMoney(-100);
                            player1.unlock();
                            break;
                        case 7:
                            player1.proceed((24 - player1.getPos()) % 32);
                            break;
                        case 8:
                            player1.proceed((8 - player1.getPos()) % 32);
                            break;
                        case 9:
                            player1.addMoney(500);
                            player1.unlock();
                            break;
                        }
                    case 31:
                        player1.addMoney(-100);
                        player1.unlock();
                        break;
                    }
                }
                else {
                    if (player1.done()) {
                        if (ImGui::Button("End Turn")) {
                            player1.endTurn();
                            if (player1.getTurns() > 0) {
                                player1.decreaseTurn();
                            }
                            turn = (turn + 1) % 4;
                        }
                    }
                    else {
                        if (ImGui::Button("Roll")) {
                            int* dice = player1.roll();
                            d11 = dice[0];
                            d12 = dice[1];
                            if (player1.jailTurn() == -1) {
                                player1.proceed(d11 + d12);
                                player1.lock();
                                if (d11 != d12) {
                                    player1.end();
                                }
                            }
                            else if (player1.jailTurn() == 0) {
                                player1.breakOut();
                                player1.proceed(d11 + d12);
                                player1.lock();
                                if (d11 != d12) {
                                    player1.end();
                                }
                            }
                            else if (player1.jailTurn() > 0) {
                                if (d11 == d12) {
                                    player1.breakOut();
                                    player1.proceed(d11 + d12);
                                    player1.lock();
                                }
                                else {
                                    player1.jail();
                                    player1.end();
                                }
                            }
                        }
                    }
                }

                ImGui::BulletText("Dice Results : %d %d", d11, d12);
                ImGui::Text("Color button with Picker:");
                ImGui::ColorEdit4("MyColor##3", (float*)&color1, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | misc_flags);
                if (owner1!=nullptr) {
                    ImGui::Text("paid player %d $%d", owner1->getInd(), toll1);
                }
            }
            ImGui::End();


            static int d21, d22;
            ImGui::Begin("Player 2");
            if (player2.isBroke()) {
                ImGui::BulletText("You are broke");
                if (turn == 1)
                    turn = (turn + 1) % 4;
            }
            else
            {
                ImGui::BulletText("Money : $%d", player2.getMoney());
                ImGui::BulletText("Position : %d", player2.getPos());
                if (player2.jailTurn() > 0) {
                    ImGui::BulletText("in jail for %d turns", player2.jailTurn());
                }
                if (player2.getTurns() > 0) {
                    ImGui::BulletText("Debt is due in %d turns", player2.getTurns());
                }
                else if (player2.getTurns() == 0) {
                    ImGui::BulletText("Debt has been repaid");
                }
                if (player2.usedKey() != -1) {
                    ImGui::BulletText("%s", gold[player2.usedKey()].getDescription());
                }
                ImGui::BulletText("Assets :");
                int* Assets2 = player2.getAssets();
                static Player* owner2 = nullptr;
                static int toll2=0;
                for (int i = 0; i < 32; i++) {
                    if (Assets2[i] > 0) {
                        ImGui::Text("\t%d hotels in %s", Assets2[i] - 1, land[i].getName());
                    }
                }
                ImGui::Text("index is %d", player2.getInd());
                if (owner2 != nullptr) {
                    ImGui::Text("owner is %d", owner2->getInd());
                }

                if (turn != 1)
                    ImGui::Button("Done");
                else if (player2.isBusy() && !land[player2.getPos()].isSpecial()) {
                    int pos = player2.getPos();
                    bool owned = player1.getAssets()[pos] || player2.getAssets()[pos] || player3.getAssets()[pos] || player4.getAssets()[pos];
                    if (!owned) {
                        static int i1 = 0;
                        ImGui::SliderInt("slider int", &i1, 0, 3);

                        static int selected = -1;
                        const char* names[] = { "Yes", "No" };
                        if (ImGui::Button("Buy"))
                            ImGui::OpenPopup("my_select_popup");
                        ImGui::SameLine();
                        ImGui::TextUnformatted(selected == -1 ? "<None>" : names[selected]);
                        if (ImGui::BeginPopup("my_select_popup"))
                        {
                            ImGui::Text("It will cost $%d", landPrice + hotelPrice * i1);
                            ImGui::Separator();
                            for (int i = 0; i < IM_ARRAYSIZE(names); i++)
                                if (ImGui::Selectable(names[i]))
                                    selected = i;
                            ImGui::EndPopup();
                        }
                        if (selected == 0) {
                            static Player* pt2 = &player2;
                            if (player2.buy(player2.getPos(), i1)) {
                                land[player2.getPos()].setPlayer(pt2);
                                land[player2.getPos()].setColor(color2);
                            }
                            player2.unlock();
                        }
                        else if (selected == 1) {
                            player2.unlock();
                        }
                        selected = -1;
                    }
                    else if (player2.getAssets()[pos] > 0) {
                        static int i1 = 0;
                        
                        ImGui::SliderInt("slider int", &i1, 0, 3 - player2.getAssets()[pos] + 1);

                        static int selected = -1;
                        const char* names[] = { "Yes", "No" };
                        if (ImGui::Button("Buy"))
                            ImGui::OpenPopup("my_select_popup");
                        ImGui::SameLine();
                        ImGui::TextUnformatted(selected == -1 ? "<None>" : names[selected]);
                        if (ImGui::BeginPopup("my_select_popup"))
                        {
                            ImGui::Text("It will cost $%d", hotelPrice * i1);
                            ImGui::Separator();
                            for (int i = 0; i < IM_ARRAYSIZE(names); i++)
                                if (ImGui::Selectable(names[i]))
                                    selected = i;
                            ImGui::EndPopup();
                        }
                        if (selected == 0) {
                            static Player* pt2 = &player2;
                            if (player2.buy(player2.getPos(), i1)) {
                                land[player2.getPos()].setPlayer(pt2);
                            }
                            player2.unlock();
                        }
                        else if (selected == 1) {
                            player2.unlock();
                        }
                        selected = -1;
                    }
                    else if (player1.getAssets()[pos] > 0) {
                        int h = player1.getAssets()[pos];
                        toll2 = player2.pay(&player1, h - 1);
                        if (toll2 > 0)
                        {
                            owner2 = &player1;
                        }
                        player2.unlock();
                    }
                    else if (player3.getAssets()[pos] > 0) {
                        int h = player3.getAssets()[pos];
                        toll2 = player2.pay(&player3, h - 1);
                        if (toll2 > 0)
                        {
                            owner2 = &player3;
                        }
                        player2.unlock();
                    }
                    else if (player4.getAssets()[pos] > 0) {
                        int h = player4.getAssets()[pos];
                        toll2 = player2.pay(&player4, h - 1);
                        if (toll2 > 0)
                        {
                            owner2 = &player4;
                        }
                        player2.unlock();
                    }
                }
                else if (player2.isBusy() && land[player2.getPos()].isSpecial()) {
                    int p2 = player2.getPos();
                    switch (p2)
                    {
                    case 0:
                        player2.unlock();
                        break;
                    case 4:
                        player2.addMoney(-100);
                        player2.unlock();
                        break;
                    case 8:
                        if (player2.jailTurn() < 0) {
                            player2.Jail();
                        }
                        player2.unlock();
                        break;
                    case 12:
                        if (gNum == 0) {
                            break;
                        }
                        g = player2.drawGoldKey(gold, gNum);
                        gNum--;

                        switch (g)
                        {
                        case 0:
                            player2.unlock();
                            break;
                        case 1:
                            player2.unlock();
                            break;
                        case 2:
                            player2.backwards(2);
                            break;
                        case 3:
                            player1.backwards(3);
                            break;
                        case 4:
                            player2.proceed((30 - player2.getPos()) % 32);
                            break;
                        case 5:
                            player2.proceed((19 - player2.getPos()) % 32);
                            break;
                        case 6:
                            player2.addMoney(-100);
                            player2.unlock();
                            break;
                        case 7:
                            player2.proceed((24 - player2.getPos()) % 32);
                            break;
                        case 8:
                            player2.proceed((8 - player2.getPos()) % 32);
                            break;
                        case 9:
                            player2.addMoney(500);
                            player2.unlock();
                            break;
                        }
                    case 16:
                        player2.proceed(16);
                        player2.unlock();
                        break;
                    case 20:
                        player2.addMoney(200);
                        player2.unlock();
                        break;
                    case 24:
                    {
                        static int i1 = 0;
                        ImGui::SliderInt("slider int", &i1, 0, 32);

                        static int selected = -1;
                        const char* names[] = { "Yes", "No" };
                        if (ImGui::Button("Go"))
                            ImGui::OpenPopup("my_select_popup");
                        ImGui::SameLine();
                        ImGui::TextUnformatted(selected == -1 ? "<None>" : names[selected]);
                        if (ImGui::BeginPopup("my_select_popup"))
                        {
                            int go = (p2 + i1) % 32;
                            ImGui::Text("Going to %s", land[go].getName());
                            ImGui::Separator();
                            for (int i = 0; i < IM_ARRAYSIZE(names); i++)
                                if (ImGui::Selectable(names[i]))
                                    selected = i;
                            ImGui::EndPopup();
                        }
                        if (selected == 0) {
                            player2.proceed(i1);
                        }
                        selected = -1;
                        break;
                    }
                    case 28:
                        if (gNum == 0) {
                            break;
                        }
                        g = player2.drawGoldKey(gold, gNum);
                        gNum--;

                        switch (g)
                        {
                        case 0:
                            player2.unlock();
                            break;
                        case 1:
                            player2.unlock();
                            break;
                        case 2:
                            player2.backwards(2);
                            break;
                        case 3:
                            player2.backwards(3);
                            break;
                        case 4:
                            player2.proceed((30 - player2.getPos()) % 32);
                            break;
                        case 5:
                            player2.proceed((19 - player2.getPos()) % 32);
                            break;
                        case 6:
                            player2.addMoney(-100);
                            player2.unlock();
                            break;
                        case 7:
                            player2.proceed((24 - player2.getPos()) % 32);
                            break;
                        case 8:
                            player2.proceed((8 - player2.getPos()) % 32);
                            break;
                        case 9:
                            player2.addMoney(500);
                            player2.unlock();
                            break;
                        }
                    case 31:
                        player2.addMoney(-100);
                        player2.unlock();
                        break;
                    }
                }
                else {
                    if (player2.done()) {
                        if (ImGui::Button("End Turn")) {
                            player2.endTurn();
                            if (player2.getTurns() > 0) {
                                player2.decreaseTurn();
                            }
                            turn = (turn + 1) % 4;
                        }
                    }
                    else {
                        if (ImGui::Button("Roll")) {
                            int* dice = player2.roll();
                            d21 = dice[0];
                            d22 = dice[1];
                            if (player2.jailTurn() == -1) {
                                player2.proceed(d21 + d22);
                                player2.lock();
                                if (d21 != d22) {
                                    player2.end();
                                }
                            }
                            else if (player2.jailTurn() == 0) {
                                player2.breakOut();
                                player2.proceed(d21 + d22);
                                player2.lock();
                                if (d21 != d22) {
                                    player2.end();
                                }
                            }
                            else if (player2.jailTurn() > 0) {
                                if (d21 == d22) {
                                    player2.breakOut();
                                    player2.proceed(d21 + d22);
                                    player2.lock();
                                }
                                else {
                                    player2.jail();
                                    player2.end();
                                }
                            }
                        }
                    }
                }
                ImGui::BulletText("Dice Results : %d %d", d21, d22);
                ImGui::Text("Color button with Picker:");
                ImGui::ColorEdit4("MyColor##3", (float*)&color2, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | misc_flags);
                if (owner2 != nullptr) {
                    ImGui::Text("paid player %d $%d", owner2->getInd(), toll2);
                }
            }
            ImGui::End();


            static int d31, d32;
            ImGui::Begin("Player 3");
            if (player3.isBroke()) {
                ImGui::BulletText("You are broke");
                if (turn == 2)
                    turn = (turn + 1) % 4;
            }
            else
            {
                ImGui::BulletText("Money : $%d", player3.getMoney());
                ImGui::BulletText("Position : %d", player3.getPos());
                if (player3.jailTurn() > 0) {
                    ImGui::BulletText("in jail for %d turns", player3.jailTurn());
                }
                if (player3.getTurns() > 0) {
                    ImGui::BulletText("Debt is due in %d turns", player3.getTurns());
                }
                else if (player3.getTurns() == 0) {
                    ImGui::BulletText("Debt has been repaid");
                }
                if (player3.usedKey() != -1) {
                    ImGui::BulletText("%s", gold[player3.usedKey()].getDescription());
                }
                ImGui::BulletText("Assets :");
                int* Assets3 = player3.getAssets();
                static Player* owner3 = nullptr;
                static int toll3=0;
                for (int i = 0; i < 32; i++) {
                    if (Assets3[i] > 0) {
                        ImGui::Text("\t%d hotels in %s", Assets3[i] - 1, land[i].getName());
                    }
                }
                ImGui::Text("index is %d", player3.getInd());
                if (owner3 != nullptr) {
                    ImGui::Text("owner is %d", owner3->getInd());
                }

                if (turn != 2)
                    ImGui::Button("Done");
                else if (player3.isBusy() && !land[player3.getPos()].isSpecial()) {
                    int pos = player3.getPos();
                    bool owned = player1.getAssets()[pos] || player2.getAssets()[pos] || player3.getAssets()[pos] || player4.getAssets()[pos];
                    if (!owned) {
                        static int i1 = 0;
                        ImGui::SliderInt("slider int", &i1, 0, 3);

                        static int selected = -1;
                        const char* names[] = { "Yes", "No" };
                        if (ImGui::Button("Buy"))
                            ImGui::OpenPopup("my_select_popup");
                        ImGui::SameLine();
                        ImGui::TextUnformatted(selected == -1 ? "<None>" : names[selected]);
                        if (ImGui::BeginPopup("my_select_popup"))
                        {
                            ImGui::Text("It will cost $%d", landPrice + hotelPrice * i1);
                            ImGui::Separator();
                            for (int i = 0; i < IM_ARRAYSIZE(names); i++)
                                if (ImGui::Selectable(names[i]))
                                    selected = i;
                            ImGui::EndPopup();
                        }
                        if (selected == 0) {
                            static Player* pt3 = &player3;
                            if (player3.buy(player3.getPos(), i1)) {
                                land[player3.getPos()].setPlayer(pt3);
                                land[player3.getPos()].setColor(color3);
                            }
                            player3.unlock();
                        }
                        else if (selected == 1) {
                            player3.unlock();
                        }
                        selected = -1;
                    }
                    else if (player3.getAssets()[pos] > 0) {
                        static int i1 = 0;
                        
                        ImGui::SliderInt("slider int", &i1, 0, 3 - player3.getAssets()[pos] + 1);

                        static int selected = -1;
                        const char* names[] = { "Yes", "No" };
                        if (ImGui::Button("Buy"))
                            ImGui::OpenPopup("my_select_popup");
                        ImGui::SameLine();
                        ImGui::TextUnformatted(selected == -1 ? "<None>" : names[selected]);
                        if (ImGui::BeginPopup("my_select_popup"))
                        {
                            ImGui::Text("It will cost $%d", hotelPrice * i1);
                            ImGui::Separator();
                            for (int i = 0; i < IM_ARRAYSIZE(names); i++)
                                if (ImGui::Selectable(names[i]))
                                    selected = i;
                            ImGui::EndPopup();
                        }
                        if (selected == 0) {
                            static Player* pt3 = &player3;
                            if (player3.buy(player3.getPos(), i1)) {
                                land[player3.getPos()].setPlayer(pt3);
                            }
                            player3.unlock();
                        }
                        else if (selected == 1) {
                            player3.unlock();
                        }
                        selected = -1;
                    }
                    else if (player2.getAssets()[pos] > 0) {
                        int h = player2.getAssets()[pos];
                        toll3 = player3.pay(&player2, h - 1);
                        if (toll3 > 0)
                        {
                            owner3 = &player2;
                        }
                        player3.unlock();
                    }
                    else if (player1.getAssets()[pos] > 0) {
                        int h = player1.getAssets()[pos];
                        toll3 = player3.pay(&player1, h - 1);
                        if (toll3 > 0)
                        {
                            owner3 = &player1;
                        }
                        player3.unlock();
                    }
                    else if (player4.getAssets()[pos] > 0) {
                        int h = player4.getAssets()[pos];
                        toll3 = player3.pay(&player4, h - 1);
                        if (toll3 > 0)
                        {
                            owner3 = &player4;
                        }
                        player3.unlock();
                    }
                }
                else if (player3.isBusy() && land[player3.getPos()].isSpecial()) {
                    int p3 = player3.getPos();
                    switch (p3)
                    {
                    case 0:
                        player3.unlock();
                        break;
                    case 4:
                        player3.addMoney(-100);
                        player3.unlock();
                        break;
                    case 8:
                        if (player3.jailTurn() < 0) {
                            player3.Jail();
                        }
                        player3.unlock();
                        break;
                    case 12:
                        if (gNum == 0) {
                            break;
                        }
                        g = player3.drawGoldKey(gold, gNum);
                        gNum--;

                        switch (g)
                        {
                        case 0:
                            player3.unlock();
                            break;
                        case 1:
                            player3.unlock();
                            break;
                        case 2:
                            player3.backwards(2);
                            break;
                        case 3:
                            player3.backwards(3);
                            break;
                        case 4:
                            player3.proceed((30 - player3.getPos()) % 32);
                            break;
                        case 5:
                            player3.proceed((19 - player3.getPos()) % 32);
                            break;
                        case 6:
                            player3.addMoney(-100);
                            player3.unlock();
                            break;
                        case 7:
                            player3.proceed((24 - player3.getPos()) % 32);
                            break;
                        case 8:
                            player3.proceed((8 - player3.getPos()) % 32);
                            break;
                        case 9:
                            player3.addMoney(500);
                            player3.unlock();
                            break;
                        }
                    case 16:
                        player3.proceed(16);
                        player3.unlock();
                        break;
                    case 20:
                        player3.addMoney(200);
                        player3.unlock();
                        break;
                    case 24:
                    {
                        static int i1 = 0;
                        ImGui::SliderInt("slider int", &i1, 0, 32);

                        static int selected = -1;
                        const char* names[] = { "Yes", "No" };
                        if (ImGui::Button("Go"))
                            ImGui::OpenPopup("my_select_popup");
                        ImGui::SameLine();
                        ImGui::TextUnformatted(selected == -1 ? "<None>" : names[selected]);
                        if (ImGui::BeginPopup("my_select_popup"))
                        {
                            int go = (p3 + i1) % 32;
                            ImGui::Text("Going to %s", land[go].getName());
                            ImGui::Separator();
                            for (int i = 0; i < IM_ARRAYSIZE(names); i++)
                                if (ImGui::Selectable(names[i]))
                                    selected = i;
                            ImGui::EndPopup();
                        }
                        if (selected == 0) {
                            player3.proceed(i1);
                        }
                        selected = -1;
                        break;
                    }
                    case 28:
                        if (gNum == 0) {
                            break;
                        }
                        g = player3.drawGoldKey(gold, gNum);
                        gNum--;

                        switch (g)
                        {
                        case 0:
                            player3.unlock();
                            break;
                        case 1:
                            player3.unlock();
                            break;
                        case 2:
                            player3.backwards(2);
                            break;
                        case 3:
                            player3.backwards(3);
                            break;
                        case 4:
                            player3.proceed((30 - player3.getPos()) % 32);
                            break;
                        case 5:
                            player3.proceed((19 - player3.getPos()) % 32);
                            break;
                        case 6:
                            player3.addMoney(-100);
                            player3.unlock();
                            break;
                        case 7:
                            player3.proceed((24 - player3.getPos()) % 32);
                            break;
                        case 8:
                            player3.proceed((8 - player3.getPos()) % 32);
                            break;
                        case 9:
                            player3.addMoney(500);
                            player3.unlock();
                            break;
                        }
                    case 31:
                        player3.addMoney(-100);
                        player3.unlock();
                        break;
                    }
                }
                else {
                    if (player3.done()) {
                        if (ImGui::Button("End Turn")) {
                            player3.endTurn();
                            if (player3.getTurns() > 0) {
                                player3.decreaseTurn();
                            }
                            turn = (turn + 1) % 4;
                        }
                    }
                    else {
                        if (ImGui::Button("Roll")) {
                            int* dice = player3.roll();
                            d31 = dice[0];
                            d32 = dice[1];
                            if (player3.jailTurn() == -1) {
                                player3.proceed(d31 + d32);
                                player3.lock();
                                if (d31 != d32) {
                                    player3.end();
                                }
                            }
                            else if (player3.jailTurn() == 0) {
                                player3.breakOut();
                                player3.proceed(d31 + d32);
                                player3.lock();
                                if (d31 != d32) {
                                    player3.end();
                                }
                            }
                            else if (player3.jailTurn() > 0) {
                                if (d31 == d32) {
                                    player3.breakOut();
                                    player3.proceed(d31 + d32);
                                    player3.lock();
                                }
                                else {
                                    player3.jail();
                                    player3.end();
                                }
                            }
                        }
                    }
                }

                ImGui::BulletText("Dice Results : %d %d", d31, d32);
                ImGui::Text("Color button with Picker:");
                ImGui::ColorEdit4("MyColor##3", (float*)&color3, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | misc_flags);
                if (owner3 != nullptr) {
                    ImGui::Text("paid player %d $%d", owner3->getInd(), toll3);
                }
            }
            ImGui::End();


            static int d41, d42;
            ImGui::Begin("Player 4");
            if (player4.isBroke()) {
                ImGui::BulletText("You are broke");
                if (turn == 3)
                    turn = (turn + 1) % 4;
            }
            else
            {
                ImGui::BulletText("Money : $%d", player4.getMoney());
                ImGui::BulletText("Position : %d", player4.getPos());
                if (player4.jailTurn() > 0) {
                    ImGui::BulletText("in jail for %d turns", player4.jailTurn());
                }
                if (player4.getTurns() > 0) {
                    ImGui::BulletText("Debt is due in %d turns", player4.getTurns());
                }
                else if (player4.getTurns() == 0) {
                    ImGui::BulletText("Debt has been repaid");
                }
                if (player4.usedKey() != -1) {
                    ImGui::BulletText("%s", gold[player4.usedKey()].getDescription());
                }
                ImGui::BulletText("Assets :");
                int* Assets4 = player4.getAssets();
                static Player* owner4 = nullptr;
                static int toll4=0;
                for (int i = 0; i < 32; i++) {
                    if (Assets4[i] > 0) {
                        ImGui::Text("\t%d hotels in %s", Assets4[i] - 1, land[i].getName());
                    }
                }
                ImGui::Text("index is %d", player4.getInd());
                if (owner4 != nullptr) {
                    ImGui::Text("owner is %d", owner4->getInd());
                }

                if (turn != 3)
                    ImGui::Button("Done");
                else if (player4.isBusy() && !land[player4.getPos()].isSpecial()) {
                    int pos = player4.getPos();
                    bool owned = player1.getAssets()[pos] || player2.getAssets()[pos] || player3.getAssets()[pos] || player4.getAssets()[pos];
                    if (!owned) {
                        static int i1 = 0;
                        ImGui::SliderInt("slider int", &i1, 0, 3);

                        static int selected = -1;
                        const char* names[] = { "Yes", "No" };
                        if (ImGui::Button("Buy"))
                            ImGui::OpenPopup("my_select_popup");
                        ImGui::SameLine();
                        ImGui::TextUnformatted(selected == -1 ? "<None>" : names[selected]);
                        if (ImGui::BeginPopup("my_select_popup"))
                        {
                            ImGui::Text("It will cost $%d", landPrice + hotelPrice * i1);
                            ImGui::Separator();
                            for (int i = 0; i < IM_ARRAYSIZE(names); i++)
                                if (ImGui::Selectable(names[i]))
                                    selected = i;
                            ImGui::EndPopup();
                        }
                        if (selected == 0) {
                            static Player* pt4 = &player4;
                            if (player4.buy(player4.getPos(), i1)) {
                                land[player4.getPos()].setPlayer(pt4);
                                land[player4.getPos()].setColor(color4);
                            }
                            player4.unlock();
                        }
                        else if (selected == 1) {
                            player4.unlock();
                        }
                        selected = -1;
                    }
                    else if (player4.getAssets()[pos] > 0) {
                        static int i1 = 0;
                        
                        ImGui::SliderInt("slider int", &i1, 0, 3 - player4.getAssets()[pos] + 1);

                        static int selected = -1;
                        const char* names[] = { "Yes", "No" };
                        if (ImGui::Button("Buy"))
                            ImGui::OpenPopup("my_select_popup");
                        ImGui::SameLine();
                        ImGui::TextUnformatted(selected == -1 ? "<None>" : names[selected]);
                        if (ImGui::BeginPopup("my_select_popup"))
                        {
                            ImGui::Text("It will cost $%d", hotelPrice * i1);
                            ImGui::Separator();
                            for (int i = 0; i < IM_ARRAYSIZE(names); i++)
                                if (ImGui::Selectable(names[i]))
                                    selected = i;
                            ImGui::EndPopup();
                        }
                        if (selected == 0) {
                            static Player* pt4 = &player4;
                            if (player4.buy(player4.getPos(), i1)) {
                                land[player4.getPos()].setPlayer(pt4);
                            }
                            player4.unlock();
                        }
                        else if (selected == 1) {
                            player4.unlock();
                        }
                        selected = -1;
                    }
                    else if (player2.getAssets()[pos] > 0) {
                        int h = player2.getAssets()[pos];
                        toll4 = player4.pay(&player2, h - 1);
                        if (toll4 > 0)
                        {
                            owner4 = &player2;
                        }
                        player4.unlock();
                    }
                    else if (player3.getAssets()[pos] > 0) {
                        int h = player3.getAssets()[pos];
                        toll4 = player4.pay(&player3, h - 1);
                        if (toll4 > 0)
                        {
                            owner4 = &player3;
                        }
                        player4.unlock();
                    }
                    else if (player1.getAssets()[pos] > 0) {
                        int h = player1.getAssets()[pos];
                        toll4 = player4.pay(&player1, h - 1);
                        if (toll4 > 0)
                        {
                            owner4 = &player1;
                        }
                        player4.unlock();
                    }
                }
                else if (player4.isBusy() && land[player4.getPos()].isSpecial()) {
                    int p4 = player4.getPos();
                    switch (p4)
                    {
                    case 0:
                        player4.unlock();
                        break;
                    case 4:
                        player4.addMoney(-100);
                        player4.unlock();
                        break;
                    case 8:
                        if (player4.jailTurn() < 0) {
                            player4.Jail();
                        }
                        player4.unlock();
                        break;
                    case 12:
                        if (gNum == 0) {
                            break;
                        }
                        g = player4.drawGoldKey(gold, gNum);
                        gNum--;

                        switch (g)
                        {
                        case 0:
                            player4.unlock();
                            break;
                        case 1:
                            player4.unlock();
                            break;
                        case 2:
                            player4.backwards(2);
                            break;
                        case 3:
                            player4.backwards(3);
                            break;
                        case 4:
                            player4.proceed((30 - player4.getPos()) % 32);
                            break;
                        case 5:
                            player4.proceed((19 - player4.getPos()) % 32);
                            break;
                        case 6:
                            player4.addMoney(-100);
                            player4.unlock();
                            break;
                        case 7:
                            player4.proceed((24 - player4.getPos()) % 32);
                            break;
                        case 8:
                            player4.proceed((8 - player4.getPos()) % 32);
                            break;
                        case 9:
                            player4.addMoney(500);
                            player4.unlock();
                            break;
                        }
                    case 16:
                        player4.proceed(16);
                        player4.unlock();
                        break;
                    case 20:
                        player4.addMoney(200);
                        player4.unlock();
                        break;
                    case 24:
                    {
                        static int i1 = 0;
                        ImGui::SliderInt("slider int", &i1, 0, 32);

                        static int selected = -1;
                        const char* names[] = { "Yes", "No" };
                        if (ImGui::Button("Go"))
                            ImGui::OpenPopup("my_select_popup");
                        ImGui::SameLine();
                        ImGui::TextUnformatted(selected == -1 ? "<None>" : names[selected]);
                        if (ImGui::BeginPopup("my_select_popup"))
                        {
                            int go = (p4 + i1) % 32;
                            ImGui::Text("Going to %s", land[go].getName());
                            ImGui::Separator();
                            for (int i = 0; i < IM_ARRAYSIZE(names); i++)
                                if (ImGui::Selectable(names[i]))
                                    selected = i;
                            ImGui::EndPopup();
                        }
                        if (selected == 0) {
                            player4.proceed(i1);
                        }
                        selected = -1;
                        break;
                    }
                    case 28:
                        if (gNum == 0) {
                            break;
                        }
                        g = player4.drawGoldKey(gold, gNum);
                        gNum--;

                        switch (g)
                        {
                        case 0:
                            player4.unlock();
                            break;
                        case 1:
                            player4.unlock();
                            break;
                        case 2:
                            player4.backwards(2);
                            break;
                        case 3:
                            player4.backwards(3);
                            break;
                        case 4:
                            player4.proceed((30 - player4.getPos()) % 32);
                            break;
                        case 5:
                            player4.proceed((19 - player4.getPos()) % 32);
                            break;
                        case 6:
                            player4.addMoney(-100);
                            player4.unlock();
                            break;
                        case 7:
                            player4.proceed((24 - player4.getPos()) % 32);
                            break;
                        case 8:
                            player4.proceed((8 - player4.getPos()) % 32);
                            break;
                        case 9:
                            player4.addMoney(500);
                            player4.unlock();
                            break;
                        }
                    case 31:
                        player4.addMoney(-100);
                        player4.unlock();
                        break;
                    }
                }
                else {
                    if (player4.done()) {
                        if (ImGui::Button("End Turn")) {
                            player4.endTurn();
                            if (player4.getTurns() > 0) {
                                player4.decreaseTurn();
                            }
                            turn = (turn + 1) % 4;
                        }
                    }
                    else {
                        if (ImGui::Button("Roll")) {
                            int* dice = player4.roll();
                            d41 = dice[0];
                            d42 = dice[1];
                            if (player4.jailTurn() == -1) {
                                player4.proceed(d41 + d42);
                                player4.lock();
                                if (d41 != d42) {
                                    player4.end();
                                }
                            }
                            else if (player4.jailTurn() == 0) {
                                player4.breakOut();
                                player4.proceed(d41 + d42);
                                player4.lock();
                                if (d41 != d42) {
                                    player4.end();
                                }
                            }
                            else if (player4.jailTurn() > 0) {
                                if (d41 == d42) {
                                    player4.breakOut();
                                    player4.proceed(d41 + d42);
                                    player4.lock();
                                }
                                else {
                                    player4.jail();
                                    player4.end();
                                }
                            }
                        }
                    }
                }

                ImGui::BulletText("Dice Results : %d %d", d41, d42);
                ImGui::Text("Color button with Picker:");
                ImGui::ColorEdit4("MyColor##3", (float*)&color4, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | misc_flags);
                if (owner4 != nullptr) {
                    ImGui::Text("paid player %d $%d", owner4->getInd(), toll4);
                }
            }
            ImGui::End();

            if (!player1.isBroke()) {
                int land1 = player1.getPos();
                ImVec2 pos1 = land[land1].getPos();
                ImVec2 pos11 = ImVec2(pos1.x + offset2.x, pos1.y + offset2.y);
                draw_list->AddNgonFilled(pos11, sz * 0.2f, ImColor(color1), ngon_sides);
            }
            if (!player2.isBroke()) {
                int land2 = player2.getPos();
                ImVec2 pos2 = land[land2].getPos();
                ImVec2 pos22 = ImVec2(pos2.x + 2 * offset2.x, pos2.y + offset2.y);
                draw_list->AddNgonFilled(pos22, sz * 0.2f, ImColor(color2), ngon_sides);
            }
            if (!player3.isBroke()) {
                int land3 = player3.getPos();
                ImVec2 pos3 = land[land3].getPos();
                ImVec2 pos33 = ImVec2(pos3.x + 3 * offset2.x, pos3.y + offset2.y);
                draw_list->AddNgonFilled(pos33, sz * 0.2f, ImColor(color3), ngon_sides);
            }
            if (!player4.isBroke()) {
                int land4 = player4.getPos();
                ImVec2 pos4 = land[land4].getPos();
                ImVec2 pos44 = ImVec2(pos4.x + 4 * offset2.x, pos4.y + offset2.y);
                draw_list->AddNgonFilled(pos44, sz * 0.2f, ImColor(color4), ngon_sides);
            }


            int* Assets1 = player1.getAssets();
            int* Assets2 = player2.getAssets();
            int* Assets3 = player3.getAssets();
            int* Assets4 = player4.getAssets();

            for (int i = 0; i < 32; i++) {
                int n = Assets1[i];
                ImVec2 p1 = land[i].getPos();
                for (int j = 0; j < n; j++) {
                    draw_list->AddCircleFilled(ImVec2(p1.x+offset2.x*1.2f*j+7,p1.y+offset2.y*0.6f), sz * 0.2f, ImColor(color1), circle_segments);
                }
            }
            for (int i = 0; i < 32; i++) {
                int n = Assets2[i];
                ImVec2 p1 = land[i].getPos();
                for (int j = 0; j < n; j++) {
                    draw_list->AddCircleFilled(ImVec2(p1.x + offset2.x *1.2f* j+7, p1.y + offset2.y * 0.6f), sz * 0.2f, ImColor(color2), circle_segments);
                }
            }
            for (int i = 0; i < 32; i++) {
                int n = Assets3[i];
                ImVec2 p1 = land[i].getPos();
                for (int j = 0; j < n; j++) {
                    draw_list->AddCircleFilled(ImVec2(p1.x + offset2.x*1.2f * j+7, p1.y + offset2.y * 0.6f), sz * 0.2f, ImColor(color3), circle_segments);
                }
            }
            for (int i = 0; i < 32; i++) {
                int n = Assets4[i];
                ImVec2 p1 = land[i].getPos();
                for (int j = 0; j < n; j++) {
                    draw_list->AddCircleFilled(ImVec2(p1.x + offset2.x*1.2f * j+7, p1.y + offset2.y * 0.6f), sz * 0.2f, ImColor(color4), circle_segments);
                }
            }
        }

            

        ImGui::End();





        ImGui::End();

    }

}
