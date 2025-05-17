import json
import unreal
from typing import Dict, Any
from utils import logging as log


def handle_create_material_node(command: Dict[str, Any]) -> Dict[str, Any]:
    """Create a material expression node on an existing material"""
    try:
        material_path = command.get("material_path")
        node_type = command.get("node_type")
        node_position = command.get("node_position", [0, 0])
        node_properties = command.get("node_properties", {})

        if not material_path or not node_type:
            return {"success": False, "error": "Missing required parameters"}

        props_json = json.dumps(node_properties)
        result_json = unreal.GenMaterialUtils.add_material_node(
            material_path, node_type, float(node_position[0]), float(node_position[1]), props_json
        )
        return json.loads(result_json)
    except Exception as e:
        log.log_error(f"Error creating material node: {str(e)}", include_traceback=True)
        return {"success": False, "error": str(e)}


def handle_get_material_nodes(command: Dict[str, Any]) -> Dict[str, Any]:
    """Return list of all available material nodes and their editable properties"""
    try:
        json_str = unreal.GenMaterialUtils.get_all_material_nodes()
        data = json.loads(json_str)
        return {"success": True, "nodes": data}
    except Exception as e:
        log.log_error(f"Error fetching material nodes: {str(e)}", include_traceback=True)
        return {"success": False, "error": str(e)}

