"""Fail when the reusable core/services acquire platform or GUI includes."""
from pathlib import Path
import re

root = Path(__file__).resolve().parents[1]
errors = []
for folder in ('core', 'services'):
    for path in (root / folder).rglob('*'):
        if path.suffix not in ('.c', '.h'):
            continue
        for name in re.findall(r'^\s*#\s*include\s*[<"]([^>"]+)', path.read_text(encoding='utf-8'), re.M):
            if name.startswith(('esp_', 'freertos/', 'driver/', 'lvgl', 'board', 'sdkconfig', 'goldie_')):
                errors.append(f'{path.relative_to(root)} imports {name}')
if errors:
    raise SystemExit('\n'.join(errors))
print('Architecture check passed: core/services have no platform, board or GUI imports.')
