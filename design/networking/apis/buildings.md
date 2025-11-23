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