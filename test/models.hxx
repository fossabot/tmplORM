#include <tmplORM.hxx>
#include "serializer/json.hxx"

namespace models
{
	using namespace tmplORM;

	struct category_t : public model_t<ts("Categories"),
		types::autoInc_t<types::primary_t<types::int32_t<ts("CategoryID")>>>, types::unicode_t<ts("CategoryName"), 15>,
		types::jsonNullable_t<types::unicodeText_t<ts("Description")>>//, types::jsonNullable_t<types::binary_t<ts("Picture")>>
	> { };

	struct supplier_t : public model_t<ts("Suppliers"),
		types::autoInc_t<types::primary_t<types::int32_t<ts("SupplierID")>>>, types::unicode_t<ts("CompanyName"), 40>,
		types::jsonNullable_t<types::unicode_t<ts("ContactName"), 30>>, types::jsonNullable_t<types::unicode_t<ts("ContactTitle"), 30>>,
		types::jsonNullable_t<types::unicode_t<ts("Address"), 60>>, types::jsonNullable_t<types::unicode_t<ts("City"), 15>>,
		types::jsonNullable_t<types::unicode_t<ts("Region"), 15>>, types::jsonNullable_t<types::unicode_t<ts("PostalCode"), 10>>,
		types::jsonNullable_t<types::unicode_t<ts("Country"), 15>>, types::jsonNullable_t<types::unicode_t<ts("Phone"), 24>>,
		types::jsonNullable_t<types::unicode_t<ts("Fax"), 24>>, types::jsonNullable_t<types::unicodeText_t<ts("HomePage")>>
	> { };

	struct product_t : public model_t<ts("Products"),
		types::autoInc_t<types::primary_t<types::int32_t<ts("ProductID")>>>, types::unicode_t<ts("ProductName"), 40>,
		types::jsonNullable_t<types::int32_t<ts("SupplierID")>>, types::jsonNullable_t<types::int32_t<ts("CategoryID")>>,
		types::jsonNullable_t<types::unicode_t<ts("QuantityPerUnit"), 20>>,// types::jsonNullable_t<types:: <ts("UnitPrice")>>,
		types::jsonNullable_t<types::int16_t<ts("UnitsInStock")>>, types::jsonNullable_t<types::int16_t<ts("UnitsOnOrder")>>,
		types::jsonNullable_t<types::int16_t<ts("ReorderLevel")>>, types::bool_t<ts("Discontinued")>
	> { };

	struct customer_t : public model_t<ts("Customers"),
		types::primary_t<types::unicode_t<ts("CustomerID"), 5>>, types::unicode_t<ts("CompanyName"), 40>,
		types::jsonNullable_t<types::unicode_t<ts("ContactName"), 30>>, types::jsonNullable_t<types::unicode_t<ts("ContactTitle"), 30>>,
		types::jsonNullable_t<types::unicode_t<ts("Address"), 60>>, types::jsonNullable_t<types::unicode_t<ts("City"), 15>>,
		types::jsonNullable_t<types::unicode_t<ts("Region"), 15>>, types::jsonNullable_t<types::unicode_t<ts("PostalCode"), 10>>,
		types::jsonNullable_t<types::unicode_t<ts("Country"), 15>>, types::jsonNullable_t<types::unicode_t<ts("Phone"), 24>>,
		types::jsonNullable_t<types::unicode_t<ts("Fax"), 24>>
	> { };

	struct shipper_t : public model_t<ts("Shippers"),
		types::autoInc_t<types::primary_t<types::int32_t<ts("ShipperID")>>>, types::unicode_t<ts("CompanyName"), 40>,
		types::jsonNullable_t<types::unicode_t<ts("Phone"), 24>>
	> { };

	struct region_t : public model_t<ts("Regions"),
		types::autoInc_t<types::primary_t<types::int32_t<ts("RegionID")>>>, types::unicode_t<ts("RegionDescription"), 50>
	> { };

	struct territory_t : public model_t<ts("Territories"),
		types::primary_t<types::unicode_t<ts("TerritoryID"), 20>>, types::unicode_t<ts("TerritoryDescription"), 50>,
		types::int32_t<ts("RegionID")>
	> { };

	struct employee_t : public model_t<ts("Employees"),
		types::autoInc_t<types::primary_t<types::int32_t<ts("EmployeeID")>>>, types::unicode_t<ts("LastName"), 20>,
		types::unicode_t<ts("FirstName"), 10>, types::jsonNullable_t<types::unicode_t<ts("Title"), 30>>,
		types::jsonNullable_t<types::unicode_t<ts("TitleOfCourtesy"), 25>>, types::jsonNullable_t<types::dateTime_t<ts("BirthDate")>>,
		types::jsonNullable_t<types::dateTime_t<ts("HireDate")>>, types::jsonNullable_t<types::unicode_t<ts("Address"), 60>>,
		types::jsonNullable_t<types::unicode_t<ts("City"), 15>>, types::jsonNullable_t<types::unicode_t<ts("Region"), 15>>,
		types::jsonNullable_t<types::unicode_t<ts("PostalCode"), 10>>, types::jsonNullable_t<types::unicode_t<ts("Country"), 15>>,
		types::jsonNullable_t<types::unicode_t<ts("HomePhone"), 24>>, types::jsonNullable_t<types::unicode_t<ts("Extension"), 4>>,
		/*types::jsonNullable_t<types::binary_t<ts("Photo")>>, */types::jsonNullable_t<types::unicodeText_t<ts("Notes")>>,
		types::jsonNullable_t<types::int32_t<ts("ReportsTo")>>, types::jsonNullable_t<types::unicode_t<ts("PhotoPath"), 255>>
	> { };

	struct order_t : public model_t<ts("Orders"),
		types::autoInc_t<types::primary_t<types::int32_t<ts("OrderID")>>>, types::jsonNullable_t<types::unicode_t<ts("CustomerID"), 5>>,
		types::jsonNullable_t<types::int32_t<ts("EmployeeID")>>, types::jsonNullable_t<types::dateTime_t<ts("OrderDate")>>,
		types::jsonNullable_t<types::dateTime_t<ts("RequiredDate")>>, types::jsonNullable_t<types::dateTime_t<ts("ShippedDate")>>,
		types::jsonNullable_t<types::int32_t<ts("ShipVia")>>,// types::jsonNullable_t<types:: <ts("Freight")>>,
		types::jsonNullable_t<types::unicode_t<ts("ShipName"), 40>>, types::jsonNullable_t<types::unicode_t<ts("ShipAddress"), 60>>,
		types::jsonNullable_t<types::unicode_t<ts("ShipCity"), 15>>, types::jsonNullable_t<types::unicode_t<ts("ShipRegion"), 15>>,
		types::jsonNullable_t<types::unicode_t<ts("ShipPostalCode"), 10>>, types::jsonNullable_t<types::unicode_t<ts("ShipCountry"), 15>>
	> { };

	struct demographic_t : public model_t<ts("CustomerDemographics"),
		types::primary_t<types::unicode_t<ts("CustomerTypeID"), 10>>, types::jsonNullable_t<types::unicodeText_t<ts("CustomerDesc")>>
	> { };

	struct customerDemographic_t : public model_t<ts("CustomerCustDemographics"),
		types::primary_t<types::unicode_t<ts("CustomerID"), 5>>, types::primary_t<types::unicode_t<ts("CustomerTypeID"), 10>>
	> { };
}
