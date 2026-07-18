namespace SGToolsUI.FileSystem
{
    public enum LoadType
    {
        Load,
        LoadAs
    }

    public enum SaveType
    {
        Save,
        SaveAs
    }

    public enum ExportType
    {
        Export,
        ExportAs
    }

    public class SGUIFileSystem
    {
        public const string XmlExtension = ".xml";
        public const string UIMetaExtension = ".uimeta";
        public const string BackupDirectoryRoot = "backup";

        public const string LoadKey = "Load";
        public const string LoadAsKey = "LoadAs";
        public const string SaveKey = "Save";
        public const string SaveAsKey = "SaveAs";
        public const string ExportKey = "Export";
        public const string ExportAsKey = "ExportAs";
    }
}