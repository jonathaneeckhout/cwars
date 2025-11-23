# Units

## GET /units
List all units owned by current player.

**Query Parameters:**
- `unit_type` (optional): Filter by type (woodcutter, farmer, axe_fighter, etc.)
- `limit` (optional, default 100): Max results
- `offset` (optional, default 0): Pagination offset

**Response:** `200 OK`
```json
{
  "units": [
    {
      "unit_id": "uuid",
      "unit_type": "woodcutter",
      "position": {"x": 105, "y": 255},
      "health": 100,
      "food": 85,
      "age": 25,
      "status": "working"
    }
  ],
  "total": 42,
  "limit": 100,
  "offset": 0
}
```

## GET /units/{unit_id}
Get detailed information about a specific unit.

**Response:** `200 OK`
```json
{
  "unit_id": "uuid",
  "unit_type": "axe_fighter",
  "owner_id": "uuid",
  "position": {"x": 120, "y": 200},
  "stats": {
    "health": 100,
    "max_health": 100,
    "attack_power": 15,
    "pierce_attack": 0,
    "defense": 10,
    "movement_speed": 5,
    "range": 1,
    "food": 75,
    "age": 30,
    "max_age": 65
  },
  "current_action": "idle",
  "created_at": "timestamp"
}
```

## POST /units/move
Command units to move to a target position.

**Request:**
```json
{
    "unit_ids": ["uuid1", "uuid2"],
    "target_position": {"x": 150, "y": 300}
}
```

**Response:** `202 Accepted`
```json
{
    "command_id": "uuid",
    "status": "pending",
    "units_affected": 2,
    "message": "Movement command queued"
}
```

## POST /units/attack
Command units to attack a target (unit or building).

**Request:**
```json
{
    "unit_ids": ["uuid1", "uuid2"],
    "target_id": "uuid"
}
```

**Response:** `202 Accepted`
```json
{
    "command_id": "uuid",
    "status": "pending",
    "units_affected": 2,
    "message": "Attack command queued"
}
```

**Error Responses:**
- `400 Bad Request` - Invalid target or non-military units
- `404 Not Found` - Target doesn't exist
- `403 Forbidden` - Units don't belong to player

## POST /units/train
Train new units at a settlement.

**Request:**
```json
{
  "building_id": "uuid",
  "unit_type": "woodcutter",
  "quantity": 2
}
```

**Response:** `202 Accepted`
```json
{
  "command_id": "uuid",
  "status": "pending",
  "cost": {
    "gold": 20
  },
  "training_time": "30 seconds",
  "message": "Training queued"
}
```