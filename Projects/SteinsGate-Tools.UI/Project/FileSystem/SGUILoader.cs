﻿/*
 * 작성자: 윤정도
 * 생성일: 3/10/2023 8:57:59 AM
 *
 * 비동기로 수행한다.
 * 따라서 비동기수행중 데이터를 변경하거나 삭제하면 문제가 발생할 수 있으므로 자제하도록.
 * 동기로 하는게 안전하지만 렉걸리는게 싫어서 비동기로 다 바꿈
 * 엘리먼트마다 락을 걸면 신경안써도 되지만 코드가 복잡해질 우려가 있다. 또한, 작업 수행 성능에 영향을 줄 수도 있기 때문이다.
 *
 */

using SGToolsUI.ViewModel;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using SGToolsUI.Model;
using System.Diagnostics.Metrics;
using SGToolsCommon.Resource;
using System.Text.RegularExpressions;
using System.Xml.Linq;
using Newtonsoft.Json.Linq;
using System.Collections;
using System.Collections.ObjectModel;
using System.IO;
using System.Windows.Media.Media3D;
using SGToolsCommon;
using SGToolsCommon.Extension;
using SGToolsUI.View;

namespace SGToolsUI.FileSystem
{
    public class SGUILoader : SGUIFileSystem
    {
        public SGUILoader(MainViewModel viewModel)
            => _viewModel = viewModel;

        public bool Load(string path, SGUIGroupMaster master)
        {
            try
            {
                string fileName = Path.GetFileName(path);
                JObject root = JObject.Parse(System.IO.File.ReadAllText(path));

                SaveMode mode = (SaveMode)Enum.Parse(typeof(SaveMode), (string)root[JsonModeKey]);

                if (mode == SaveMode.GameData)
                    throw new Exception("게임 데이터를 로딩할려고 하고 있습니다.");

                JArray? elements = root[JsonElementKey] as JArray;
                JArray? groups = root[JsonGroupKey] as JArray;
                JObject? groupMaster = root[JsonGroupMasterKey] as JObject;

                List<(SGUIGroup, JArray)> groupList = new(groups.Count);   // 그룹 뼈대 로딩때 임시로 저장하는 리스트
                Dictionary<int, SGUIElement> elementDict = new();           // 그룹, 엘리먼트 저장용 임시 맵, 그룹 로딩시 자식 코드로 빠르게 엘리먼트 추가하기 위한 용도


                // Step1. 모든 그룹 제외 엘리먼트정보들 로딩
                for (int i = 0; i < elements.Count; ++i)
                {
                    JObject elementRoot = elements[i] as JObject;
                    int code = (int)elementRoot[SGUIElement.JsonCodeKey];
                    SGUIElementType type = (SGUIElementType)((int)elementRoot[SGUIElement.JsonElementTypeKey]);
                    SGUIElement element = SGUIElement.Create(type);
                    element.ViewModel = _viewModel;
                    element.ParseJObject(elementRoot);
                    elementDict.Add(code, element);
                }

                // Step2. 모든 그룹 뼈대만 로딩, 그룹간 부모/자식 관계가 존재하기 때문에 미리 만듬
                for (int i = 0; i < groups.Count; ++i)
                {
                    JObject groupRoot = groups[i] as JObject;
                    int code = (int)groupRoot[SGUIElement.JsonCodeKey];
                    JArray childrenRoot = groupRoot[SGUIElement.JsonChildrenKey] as JArray;

                    SGUIGroup group = new SGUIGroup(childrenRoot.Count);
                    group.ViewModel = _viewModel;
                    group.ParseJObject(groupRoot);
                    elementDict.Add(code, group);
                    groupList.Add((group, childrenRoot));
                }

                // Step3. 이제 모든 그룹포함 엘리먼트정보가 엘리먼트 맵에 등록되었으므로 그룹 자식등록 진행
                for (int i = 0; i < groupList.Count; ++i)
                {
                    SGUIGroup group = groupList[i].Item1;
                    foreach (JToken childInfoRoot in groupList[i].Item2)
                    {
                        int[] newChildInfo = new int[3];
                        SGUIElement newChild = ParseChildInfo(childInfoRoot, newChildInfo);
                        group.Children.Add(newChild);

                        newChild.Tag = newChildInfo;
                        newChild.Parent = group;
                    }
                }

                // Step4. 마스터 그룹 로딩, 그룹 마스터와 부모, 자식 등록진행
                ObservableCollection<SGUIElement> parsed = new ();
                JArray masterGroups = groupMaster[SGUIElement.JsonChildrenKey] as JArray;
                master.ParseJObject(groupMaster);

                for (int i = 0; i < masterGroups.Count; ++i)
                {
                    int[] newChildInfo = new int[3];
                    SGUIGroup masterGroup = ParseChildInfo(masterGroups[i], newChildInfo).Cast<SGUIGroup>();
                    masterGroup.Parent = master;
                    masterGroup.Tag = newChildInfo;
                    master.Children.Add(masterGroup);
                }

                // 모든 자식 순회하면서 기타 설정까지 한번에 수행, 이때부턴 그룹의 코드가 다시 재구성된다. 태그에 저장된 childInfo[0](코드)는 이후로 쓸모없어짐
                // Step5. 부모 관계가 모두 확립되었으므로 상위 계층부터 하위 계층까지 순회하며 RelativePosition VisualPosition으로 반영
                master.ForEachRecursive(element =>
                {
                    int[] childInfo = (int[])element.Tag;
                    element.RelativePosition = new Point(childInfo[1], childInfo[2]);

                    if (!element.IsGroup)
                    {
                        master.AddElement(element);
                        return;
                    }

                    SGUIGroup newGroup = element.Cast<SGUIGroup>();
                    master.AddGroup(newGroup);
                    newGroup.SetDepth(newGroup.Parent.Depth + 1);
                });


                SGUIElement ParseChildInfo(JToken? token, int[] arr)
                {
                    string childInfoStr = ((string)token);
                    StringEx.ParseIntNumberN(childInfoStr, arr);
                    SGUIElement newChild = elementDict[arr[0]];
                    return newChild;
                }

                _viewModel.LogBox.AddDispatchedLog($"UI툴 데이터 로딩완료 {fileName}", (LogType.Path, path), IconCommonType.Checked, Brushes.Green);
                return true;
            }
            catch (Exception exception)
            {
                _viewModel.LogBox.AddDispatchedLog(exception);
                return false;
            }
        }

        public async Task<SGUIGroupMaster> LoadAsync(string path)
        {
            SGUIGroupMaster master = SGUIGroupMaster.Create(_viewModel);
            bool result = await Task.Run(() => Load(path, master));

            if (result)
                return master;

            return SGUIGroupMaster.Create(_viewModel);
        }


        public MainViewModel _viewModel;
    }
}