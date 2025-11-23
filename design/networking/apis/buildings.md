# Buildings

## GET /buildings
List buildings owned by current player.

**Query Parameters:**
- `building_type` (optional): Filter by type

**Response:** `200 OK`
```json
{
  "buildings": [
    {
      "building_id": "uuid",
      "building_type": "woodcutters_hut",
      "position": {"x": 110, "y": 260},
      "status": "operational",
    }
  ]
}
```

## GET /buildings/{building_id}
Get detailed information about a specific building.

**Response:** `200 OK`
```json
{
  "building_id": "uuid",
  "building_type": "bakery",
  "owner_id": "uuid",
  "position": {"x": 115, "y": 265},
  "status": "operational",
  "health": 100,
  "input_storage": {
    "flour": 10
  },
  "output_storage": {
    "bread": 5
  },
  "created_at": "timestamp"
}
```

## GET /buildings/storage/{storage_id}
Get all resources stored at a storage.

**Response:** `200 OK`
```json
{
  "storage_id": "uuid",
  "storage_capacity": 1000,
  "storage_used": 450,
  "resources": {
    "gold": 150,
    "logs": 20,
    "planks": 45,
    "stones": 30,
    "grain": 50,
    "flour": 25,
    "bread": 15,
    "pigs": 5,
    "meat": 10,
    "skins": 8,
    "leather": 3,
    "coal": 12,
    "iron_ore": 8,
    "gold_ore": 3,
    "iron_bars": 2,
    "gold_bars": 1,
    "axes": 3,
    "bows": 2,
    "spears": 4,
    "wooden_shields": 5,
    "leather_armor": 2
  }
}
```

## POST /buildings/construct
Construct a new building.

**Request:**
```json
{
  "building_type": "farm",
  "position": {"x": 120, "y": 270}
}
```

**Response:** `202 Accepted`
```json
{
  "command_id": "uuid",
  "building_id": "uuid",
  "status": "pending",
  "cost": {
    "planks": 10,
    "stones": 5
  },
  "construction_time": "60 seconds",
  "message": "Construction queued"
}
```

### POST /buildings/{building_id}/construct
Assign workers to construct a building.

**Request:**
```json
{
  "unit_ids": ["uuid1", "uuid2"]
}
```

**Response:** `202 Accepted`
```json
{
  "command_id": "uuid",
  "status": "pending",
  "message": "Worker assignment queued"
}
```

### POST /buildings/{building_id}/assign
Assign unit to a building.

**Request:**
```json
{
  "unit_ids": "uuid"
}
```

**Response:** `202 Accepted`
```json
{
  "command_id": "uuid",
  "status": "pending",
  "message": "Unit assignment queued"
}
```