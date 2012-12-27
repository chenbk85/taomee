<?php
$file = 'items.xml';
if (file_exists($file)) {
	$xml = simplexml_load_file($file);
	switch ($_POST['type']) {
	case "不可买卖物品" :
		echo "<table border>";
		echo "<tr><th>ID</th><th>Name</th><th>Price</th><th>不可买卖物品</th>";
		foreach($xml->Kind as $Kind) {
			foreach($Kind->Item as $Item) {
				if($Item[Tradability] == 3) {
					echo "<tr><td>$Item[ID]</td><td>$Item[Name]</td><td>$Item[Price]</td><td>$Item[Tradability]</td>";
				}
			}
		}
		echo "</table>";

		break;
	case "不可买物品" :
		echo "<table border>";
		echo "<tr><th>ID</th><th>Name</th><th>Price</th><th>不可买物品</th>";
		foreach($xml->Kind as $Kind) {
			foreach($Kind->Item as $Item) {
				if($Item[Tradability] == 1) {
					echo "<tr><td>$Item[ID]</td><td>$Item[Name]</td><td>$Item[Price]</td><td>$Item[Tradability]</td>";
				}
			}
		}
		echo "</table>";

		break;
	case "不可卖物品" :
		echo "<table border>";
		echo "<tr><th>ID</th><th>Name</th><th>Price</th><th>不可卖物品</th>";
		foreach($xml->Kind as $Kind) {
			foreach($Kind->Item as $Item) {
				if($Item[Tradability] == 2) {
					echo "<tr><td>$Item[ID]</td><td>$Item[Name]</td><td>$Item[Price]</td><td>$Item[Tradability]</td>";
				}
			}
		}
		echo "</table>";
		break;
	case "打折物品" :
		echo "<table border>";
		echo "<tr><th>ID</th><th>Name</th><th>Price</th><th>折扣</th>";
		foreach($xml->Kind as $Kind) {
			foreach($Kind->Item as $Item) {
				if($Item[Discount]) {
					echo "<tr><td>$Item[ID]</td><td>$Item[Name]</td><td>$Item[Price]</td><td>$Item[Discount]</td>";
				}
			}
		}
		echo "</table>";
		break;
	case "VIP物品" :
		echo "<table border>";
		echo "<tr><th>ID</th><th>Name</th><th>VIP物品</th><th>VIP可否购买</th>";
		foreach($xml->Kind as $Kind) {
			foreach($Kind->Item as $Item) {
				if($Item[VipOnly]) {
					echo "<tr><td>$Item[ID]</td><td>$Item[Name]</td><td>$Item[VipOnly]</td>";
					if ($Item[VipBuyable]) {
						echo "<td>Vip可以购买</td>";
					} else {
						echo "<td>&nbsp;</td>";
					}

				}
			}
		}
		echo "</table>";
		break;

	case "全部" :
		echo "<table border>";
		echo "<tr><th>ID</th><th>名字</th><th>价格</th><th>可否买卖</th>";
		echo "<th>vip能否拥有</th>";
		echo "<th>vip能否购买</th>";
		echo "<th>折扣</th>";
		foreach($xml->Kind as $Kind) {
			foreach($Kind->Item as $Item) {
				echo "<tr>";
				echo "<td>$Item[ID]</td>";
				echo "<td>$Item[Name]</td>";
				echo "<td>$Item[Price]</td>";

				switch ($Item[Tradability]) {
				case 1:
					echo "<td>不可买物品</td>";
					break;
				case 2:
					echo "<td>不可卖物品</td>";
					break;
				case 3:
					echo "<td>不可买卖物品</td>";
					break;
				default:
					echo "<td>&nbsp;</td>";
				}

				if ($Item[VipOnly]) {
					echo "<td>Vip物品</td>";
				} else {
					echo "<td>&nbsp;</td>";
				}
				if ($Item[VipBuyable]) {
					echo "<td>Vip可以购买</td>";
				} else {
					echo "<td>&nbsp;</td>";
				}
				if ($Item[Discount] == 0.5) {
					echo "<td>半价</td>";
				} else {
					echo "<td>&nbsp;</td>";
				}

			}
		}

		echo "</table>";
		break;
	}		
} else {
	exit('Failed to open items.xml');
}


?>
