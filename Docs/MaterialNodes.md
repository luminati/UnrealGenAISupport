# Material Node Reference

This file lists all available Unreal Engine material nodes and their editable properties as obtained from `UGenMaterialUtils::GetAllMaterialNodes()`.

The included `Resources/MaterialNodes.json` is a placeholder. To generate a list for your engine version run the following in the Unreal Editor Python console:

```python
from plugins.GenerativeAISupport.Content.Python.utils import generate_material_nodes
```

This will update `Resources/MaterialNodes.json` with a JSON array of nodes. Each entry has the format:

```json
{
  "class": "MaterialExpressionMultiply",
  "properties": ["A", "B", "ConstA", "ConstB"]
}
```

You can then use the MCP command `create_material_node` with these class names and property keys.
