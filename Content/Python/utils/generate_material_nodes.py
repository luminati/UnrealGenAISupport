import json
import unreal

out_path = unreal.Paths.combine([unreal.Paths.project_plugins_dir(), "GenerativeAISupport", "Resources", "MaterialNodes.json"])
json_str = unreal.GenMaterialUtils.get_all_material_nodes()
with open(out_path, "w", encoding="utf-8") as f:
    f.write(json_str)
print(f"Material node list written to {out_path}")
